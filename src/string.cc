#include "string.hh"

#include "alloc.hh"

#include <string.h>

namespace mc
{
	// The user-defined ctor is needed because clang and gcc (< 13) misunderstand the
	// union not being trivially constructible.
	// See https://github.com/llvm/llvm-project/pull/82407 for the clang fix.
	string::string()
	: small_ {0}
	{}

	string::string(uint32_t count, char c)
	: len_ {count & ~is_large_flag}
	{
		if (len_ < small_size)
		{
			memset(small_.str_, c, len_);
			small_.str_[len_] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;
			memset(large_.str_, c, len_);
			large_.str_[len_] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::string(char const* str, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = strlen(str);
		else
			len_ = count;

		len_ &= ~is_large_flag;

		if (len_ < small_size)
		{
			memcpy(small_.str_, str, len_);
			small_.str_[len_] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str, len_);
			large_.str_[len_] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::string(string_view str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = str.size();
		else
			len_ = count;

		len_ &= ~is_large_flag;

		if (len_ < small_size)
		{
			memcpy(small_.str_, str.data() + pos, len_);
			small_.str_[len_] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str.data() + pos, len_);
			large_.str_[len_] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::string(string const& str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = str.size();
		else
			len_ = count;

		len_ &= ~is_large_flag;

		if (str.is_large())
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, str.large_.str_ + pos, len_);
				small_.str_[len_] = '\0';
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, str.large_.str_ + pos, len_);
				large_.str_[len_] = '\0';
				len_ |= is_large_flag;
			}
		}
		else
		{
			memcpy(small_.str_, str.small_.str_, len_);
			small_.str_[len_] = '\0';
		}
	}

	string::string(string&& str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = str.size() - pos;
		else
			len_ = count;

		len_ = (len_ & ~is_large_flag) | (str.len_ & is_large_flag);
		if (is_large())
		{
			large_ = str.large_;
			memmove(large_.str_, large_.str_ + pos, count);
			large_.str_[(len_ & ~is_large_flag)] = '\0';
			str.large_.str_ = nullptr;
			str.large_.cap_ = 0;
		}
		else
		{
			small_ = str.small_;
			memmove(small_.str_, small_.str_ + pos, count);
			small_.str_[len_] = '\0';
		}

		str.len_ = 0;
	}

	string::string(std::initializer_list<char> ilist)
	: len_ {static_cast<uint32_t>(ilist.size())}
	{
		if (len_ < small_size)
		{
			memcpy(small_.str_, ilist.begin(), len_);
			small_.str_[len_] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, ilist.begin(), len_);
			large_.str_[len_] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::~string()
	{
		if (is_large())
			free(large_.str_, large_.cap_ + 1, alignof(char));
	}

	bool string::empty() const
	{
		return (len_ & ~is_large_flag) == 0;
	}

	uint32_t string::size() const
	{
		return len_ & ~is_large_flag;
	}

	uint32_t string::capacity() const
	{
		return is_large() ? large_.cap_ : small_size - 1;
	}

	char* string::data() &
	{
		return is_large() ? large_.str_ : small_.str_;
	}

	char const* string::data() const&
	{
		return is_large() ? large_.str_ : small_.str_;
	}

	string::operator string_view() const
	{
		return string_view(data(), size());
	}

	char& string::operator[](uint32_t pos) &
	{
		return data()[pos];
	}

	char const& string::operator[](uint32_t pos) const&
	{
		return data()[pos];
	}

	char& string::front() &
	{
		return data()[0];
	}

	char const& string::front() const&
	{
		return data()[0];
	}

	char& string::back() &
	{
		return data()[len_ & ~is_large_flag - 1];
	}

	char const& string::back() const&
	{
		return data()[len_ & ~is_large_flag - 1];
	}

	void string::clear()
	{
		if (is_large())
			large_.str_[0] = '\0';
		else
			small_.str_[0] = '\0';

		len_ = 0 | (len_ & is_large_flag);
	}

	void string::reserve(uint32_t cap)
	{
		if (cap > small_size)
		{
			if (is_large())
			{
				char* new_str = reinterpret_cast<char*>(alloc(cap + 1, alignof(char)));
				memcpy(new_str, large_.str_, (len_ & ~is_large_flag) + 1);
				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = cap;
			}
			else
			{
				char* new_str = reinterpret_cast<char*>(alloc(cap + 1, alignof(char)));
				memcpy(new_str, small_.str_, (len_ & ~is_large_flag) + 1);
				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = cap;

				len_ |= is_large_flag;
			}
		}
	}

	void string::fit()
	{
		if (is_large())
		{
			uint32_t len = len_ & ~is_large_flag;
			if (len != large_.cap_)
			{
				char* new_str = reinterpret_cast<char*>(alloc(len + 1, alignof(char)));
				memcpy(new_str, large_.str_, (len_ & ~is_large_flag) + 1);
				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = len;
			}
		}
	}

	void string::resize(uint32_t size, char c)
	{
		size &= ~is_large_flag;
		if (size > (len_ & ~is_large_flag))
		{
			if (is_large())
			{
				if (size > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < size)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
					memset(new_str + (len_ & ~is_large_flag), c,
					       size - (len_ & ~is_large_flag));
					new_str[size] = '\0';

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memset(large_.str_ + (len_ & ~is_large_flag), c,
					       size - (len_ & ~is_large_flag));
					large_.str_[size] = '\0';
				}

				len_ = size | is_large_flag;
			}
			else if (size >= small_size)
			{
				char* new_str = reinterpret_cast<char*>(alloc(size + 1, alignof(char)));

				memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
				memset(new_str + (len_ & ~is_large_flag), c,
				       size - (len_ & ~is_large_flag));
				new_str[size] = '\0';

				large_.str_ = new_str;
				large_.cap_ = size;

				len_ = size | is_large_flag;
			}
			else
			{
				memset(small_.str_ + (len_ & ~is_large_flag), c,
				       size - (len_ & ~is_large_flag));
				small_.str_[size] = '\0';

				len_ = size;
			}
		}
		else if (is_large())
		{
			large_.str_[size] = '\0';

			len_ = size | (len_ & is_large_flag);
		}
		else
		{
			small_.str_[size] = '\0';
			len_ = size;
		}
	}

	void string::assign(uint32_t count, char c)
	{
		count &= ~is_large_flag;

		if (is_large())
		{
			if (count >= large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < count)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				free(large_.str_, large_.cap_, alignof(char));
				large_.cap_ = new_cap;
				large_.str_ = new_str;
			}
			memset(large_.str_, c, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else if (count >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(count + 1, alignof(char)));

			large_.cap_ = count;
			large_.str_ = new_str;
			memset(large_.str_, c, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else
		{
			memset(small_.str_, c, count);
			small_.str_[count] = '\0';
			len_ = count;
		}
	}

	void string::assign(char const* str, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = strlen(str);

		count &= ~is_large_flag;

		if (is_large())
		{
			if (count >= large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < count)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				free(large_.str_, large_.cap_, alignof(char));
				large_.cap_ = new_cap;
				large_.str_ = new_str;
			}
			memcpy(large_.str_, str, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else if (count >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(count + 1, alignof(char)));

			large_.cap_ = count;
			large_.str_ = new_str;
			memcpy(large_.str_, str, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, str, count);
			small_.str_[count] = '\0';
			len_ = count;
		}
	}

	void string::assign(string_view str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = str.size() - pos;

		count &= ~is_large_flag;

		if (is_large())
		{
			if (count >= large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < count)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				free(large_.str_, large_.cap_, alignof(char));
				large_.cap_ = new_cap;
				large_.str_ = new_str;
			}
			memcpy(large_.str_, str.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else if (count >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(count + 1, alignof(char)));

			large_.cap_ = count;
			large_.str_ = new_str;
			memcpy(large_.str_, str.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, str.data() + pos, count);
			small_.str_[count] = '\0';
			len_ = count;
		}
	}

	void string::assign(string const& str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = str.size() - pos;

		count &= ~is_large_flag;

		if (is_large())
		{
			if (count >= large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < count)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				free(large_.str_, large_.cap_, alignof(char));
				large_.cap_ = new_cap;
				large_.str_ = new_str;
			}
			memcpy(large_.str_, str.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else if (count >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(count + 1, alignof(char)));

			large_.cap_ = count;
			large_.str_ = new_str;
			memcpy(large_.str_, str.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, str.data() + pos, count);
			small_.str_[count] = '\0';
			len_ = count;
		}
	}

	void string::assign(std::initializer_list<char> ilist)
	{
		uint32_t str_len = static_cast<uint32_t>(ilist.size()) & ~is_large_flag;

		if (is_large())
		{
			if (str_len >= large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < str_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				free(large_.str_, large_.cap_, alignof(char));
				large_.cap_ = new_cap;
				large_.str_ = new_str;
			}
			memcpy(large_.str_, ilist.begin(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else if (str_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(str_len + 1, alignof(char)));

			large_.cap_ = str_len;
			large_.str_ = new_str;
			memcpy(large_.str_, ilist.begin(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, ilist.begin(), str_len);
			small_.str_[str_len] = '\0';
			len_ = str_len;
		}
	}

	void string::assign(string&& str)
	{
		if (is_large())
			free(large_.str_, large_.cap_ + 1, alignof(char));

		len_ = str.len_;
		if (is_large())
		{
			large_.str_ = str.large_.str_;
			large_.cap_ = str.large_.cap_;
		}
		else
		{
			memcpy(small_.str_, str.small_.str_, len_ + 1);
		}

		str.len_ = 0;
	}

	string& string::operator=(char const* str)
	{
		assign(str);
		return *this;
	}

	string& string::operator=(string_view str)
	{
		assign(str);
		return *this;
	}

	string& string::operator=(string const& str)
	{
		assign(str);
		return *this;
	}

	string& string::operator=(string&& str)
	{
		assign(static_cast<string&&>(str));
		return *this;
	}

	string& string::operator=(std::initializer_list<char> ilist)
	{
		assign(ilist);

		return *this;
	}

	void string::insert(uint32_t idx, uint32_t count, char c)
	{
		if (idx == len_)
		{
			append(count, c);
		}
		else
		{
			count &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) + count;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memset(new_str + idx, c, count);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memset(large_.str_ + idx, c, count);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memset(new_str + idx, c, count);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memset(small_.str_ + idx, c, count);

				len_ = new_len;
			}
		}
	}

	void string::insert(uint32_t idx, char const* str, uint32_t count)
	{
		if (idx == len_)
		{
			append(str, count);
		}
		else
		{
			if (count == UINT32_MAX)
				count = strlen(str);

			count &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) + count;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(new_str + idx, str, count);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(large_.str_ + idx, str, count);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(new_str + idx, str, count);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(small_.str_ + idx, str, count);

				len_ = new_len;
			}
		}
	}

	void string::insert(uint32_t idx, string const& str, uint32_t pos, uint32_t count)
	{
		if (idx == len_)
		{
			append(str, pos, count);
		}
		else
		{
			if (count == UINT32_MAX)
				count = str.size() - pos;

			count &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) + count;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(new_str + idx, str.data() + pos, count);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(large_.str_ + idx, str.data() + pos, count);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(new_str + idx, str.data() + pos, count);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(small_.str_ + idx, str.data() + pos, count);

				len_ = new_len;
			}
		}
	}

	void string::insert(uint32_t idx, string_view const& str, uint32_t pos,
	                    uint32_t count)
	{
		if (idx == len_)
		{
			append(str, pos, count);
		}
		else
		{
			if (count == UINT32_MAX)
				count = str.size() - pos;

			count &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) + count;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(new_str + idx, str.data() + pos, count);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(large_.str_ + idx, str.data() + pos, count);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(new_str + idx, str.data() + pos, count);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(small_.str_ + idx, str.data() + pos, count);

				len_ = new_len;
			}
		}
	}

	void string::insert(uint32_t idx, std::initializer_list<char> ilist)
	{
		if (idx == len_)
		{
			append(ilist);
		}
		else
		{
			uint32_t count = ilist.size() & ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) + count;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(new_str + idx, ilist.begin(), count);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count, large_.str_ + idx,
					        (len_ & ~is_large_flag) - idx + 1);
					memcpy(large_.str_ + idx, ilist.begin(), count);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(new_str + idx, ilist.begin(), count);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count, small_.str_ + idx,
				        (len_ & ~is_large_flag) - idx + 1);
				memcpy(small_.str_ + idx, ilist.begin(), count);

				len_ = new_len;
			}
		}
	}

	void string::append(uint32_t count, char c)
	{
		count &= ~is_large_flag;

		uint32_t new_len = (len_ & ~is_large_flag) + count;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				memset(new_str + (len_ & ~is_large_flag), c, count);
				new_str[new_len] = '\0';

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memset(large_.str_ + (len_ & ~is_large_flag), c, count);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			memset(new_str + (len_ & ~is_large_flag), c, count);
			new_str[new_len] = '\0';

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			memset(small_.str_ + (len_ & ~is_large_flag), c, count);
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	void string::append(char const* str, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = strlen(str);

		count &= ~is_large_flag;

		uint32_t new_len = (len_ & ~is_large_flag) + count;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				memcpy(new_str + (len_ & ~is_large_flag), str, count);

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memcpy(large_.str_ + (len_ & ~is_large_flag), str, count);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			memcpy(new_str + (len_ & ~is_large_flag), str, count);

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_ + (len_ & ~is_large_flag), str, count);
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	void string::append(string const& str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = str.size() - pos;

		count &= ~is_large_flag;

		uint32_t new_len = (len_ & ~is_large_flag) + count;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				memcpy(new_str + (len_ & ~is_large_flag), str.data() + pos, count);
				new_str[new_len] = '\0';

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memcpy(large_.str_ + (len_ & ~is_large_flag), str.data() + pos, count);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			memcpy(new_str + (len_ & ~is_large_flag), str.data() + pos, count);
			new_str[new_len] = '\0';

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_ + (len_ & ~is_large_flag), str.data() + pos, count);
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	void string::append(string_view const& str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = str.size() - pos;

		count &= ~is_large_flag;

		uint32_t new_len = (len_ & ~is_large_flag) + count;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				memcpy(new_str + (len_ & ~is_large_flag), str.data() + pos, count);
				new_str[new_len] = '\0';

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memcpy(large_.str_ + (len_ & ~is_large_flag), str.data() + pos, count);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			memcpy(new_str + (len_ & ~is_large_flag), str.data() + pos, count);
			new_str[new_len] = '\0';

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_ + (len_ & ~is_large_flag), str.data() + pos, count);
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	void string::append(std::initializer_list<char> ilist)
	{
		uint32_t count = ilist.size() & ~is_large_flag;

		uint32_t new_len = (len_ & ~is_large_flag) + count;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				memcpy(new_str + (len_ & ~is_large_flag), ilist.begin(), count);
				new_str[new_len] = '\0';

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memcpy(large_.str_ + (len_ & ~is_large_flag), ilist.begin(), count);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			memcpy(new_str + (len_ & ~is_large_flag), ilist.begin(), count);
			new_str[new_len] = '\0';

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_ + (len_ & ~is_large_flag), ilist.begin(), count);
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	string& string::operator+=(char const* str)
	{
		append(str);
		return *this;
	}

	string& string::operator+=(string_view str)
	{
		append(str);
		return *this;
	}

	string& string::operator+=(string const& str)
	{
		append(str);
		return *this;
	}

	string& string::operator+=(std::initializer_list<char> ilist)
	{
		append(ilist);
		return *this;
	}

	void string::replace(uint32_t idx, uint32_t count, uint32_t count2, char c)
	{
		if (count == 0)
		{
			insert(idx, count2, c);
		}
		else
		{
			count &= ~is_large_flag;
			count2 &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) - count + count2;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memset(new_str + idx, c, count2);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memset(large_.str_ + idx, c, count2);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memset(new_str + idx, c, count2);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memset(small_.str_ + idx, c, count2);

				len_ = new_len;
			}
		}
	}

	void string::replace(uint32_t idx, uint32_t count, char const* str, uint32_t count2)
	{
		if (idx == len_)
		{
			append(str, count2);
		}
		else
		{
			if (count2 == UINT32_MAX)
				count2 = strlen(str);

			count &= ~is_large_flag;
			count2 &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) - count + count2;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(new_str + idx, str, count2);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(large_.str_ + idx, str, count2);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(new_str + idx, str, count2);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(small_.str_ + idx, str, count2);

				len_ = new_len;
			}
		}
	}

	void string::replace(uint32_t idx, uint32_t count, string const& str, uint32_t pos,
	                     uint32_t count2)
	{
		if (idx == len_)
		{
			append(str, pos, count2);
		}
		else
		{
			if (count2 == UINT32_MAX)
				count2 = str.size() - pos;

			count &= ~is_large_flag;
			count2 &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) - count + count2;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(new_str + idx, str.data() + pos, count2);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(large_.str_ + idx, str.data() + pos, count2);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(new_str + idx, str.data() + pos, count2);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(small_.str_ + idx, str.data() + pos, count2);

				len_ = new_len;
			}
		}
	}

	void string::replace(uint32_t idx, uint32_t count, string_view const& str,
	                     uint32_t pos, uint32_t count2)
	{
		if (idx == len_)
		{
			append(str, pos, count2);
		}
		else
		{
			if (count2 == UINT32_MAX)
				count2 = str.size() - pos;

			count &= ~is_large_flag;
			count2 &= ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) - count + count2;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(new_str + idx, str.data() + pos, count2);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(large_.str_ + idx, str.data() + pos, count2);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(new_str + idx, str.data() + pos, count2);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(small_.str_ + idx, str.data() + pos, count2);

				len_ = new_len;
			}
		}
	}

	void string::replace(uint32_t idx, uint32_t count, std::initializer_list<char> ilist)
	{
		if (idx == len_)
		{
			append(ilist);
		}
		else
		{
			count &= ~is_large_flag;
			uint32_t count2 = ilist.size() & ~is_large_flag;

			uint32_t new_len = (len_ & ~is_large_flag) - count + count2;
			if (is_large())
			{
				if (new_len > large_.cap_)
				{
					uint32_t new_cap = large_.cap_;
					while (new_cap < new_len)
						new_cap *= 2;

					char* new_str =
						reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

					memcpy(new_str, large_.str_, idx);
					memmove(new_str + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(new_str + idx, ilist.begin(), count2);

					free(large_.str_, large_.cap_ + 1, alignof(char));

					large_.str_ = new_str;
					large_.cap_ = new_cap;
				}
				else
				{
					memmove(large_.str_ + idx + count2, large_.str_ + idx + count,
					        (len_ & ~is_large_flag) - idx - count + 1);
					memcpy(large_.str_ + idx, ilist.begin(), count2);
				}

				len_ = new_len | is_large_flag;
			}
			else if (new_len >= small_size)
			{
				char* new_str =
					reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

				memcpy(new_str, small_.str_, idx);
				memmove(new_str + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(new_str + idx, ilist.begin(), count2);

				large_.str_ = new_str;
				large_.cap_ = new_len;

				len_ = new_len | is_large_flag;
			}
			else
			{
				memmove(small_.str_ + idx + count2, small_.str_ + idx + count,
				        (len_ & ~is_large_flag) - idx - count + 1);
				memcpy(small_.str_ + idx, ilist.begin(), count2);

				len_ = new_len;
			}
		}
	}

	void string::erase(uint32_t idx, uint32_t count)
	{
		memmove(data() + idx, data() + idx + count,
		        (len_ & ~is_large_flag) - idx - count + 1);

		len_ = (len_ & ~is_large_flag) - count | (len_ & is_large_flag);
	}

	void string::push_back(char c)
	{
		uint32_t new_len = (len_ & ~is_large_flag) + 1;
		if (is_large())
		{
			if (new_len > large_.cap_)
			{
				uint32_t new_cap = large_.cap_;
				while (new_cap < new_len)
					new_cap *= 2;

				char* new_str =
					reinterpret_cast<char*>(alloc(new_cap + 1, alignof(char)));

				memcpy(new_str, large_.str_, (len_ & ~is_large_flag));
				new_str[(len_ & ~is_large_flag)] = c;
				new_str[new_len] = '\0';

				free(large_.str_, large_.cap_ + 1, alignof(char));

				large_.str_ = new_str;
				large_.cap_ = new_cap;
			}
			else
			{
				memset(large_.str_ + (len_ & ~is_large_flag), c, 1);
				large_.str_[new_len] = '\0';
			}

			len_ = new_len | is_large_flag;
		}
		else if (new_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(new_len + 1, alignof(char)));

			memcpy(new_str, small_.str_, (len_ & ~is_large_flag));
			new_str[(len_ & ~is_large_flag)] = c;
			new_str[new_len] = '\0';

			large_.str_ = new_str;
			large_.cap_ = new_len;

			len_ = new_len | is_large_flag;
		}
		else
		{
			small_.str_[(len_ & ~is_large_flag)] = c;
			small_.str_[new_len] = '\0';

			len_ = new_len;
		}
	}

	void string::pop_back()
	{
		data()[(len_ & ~is_large_flag) - 1] = '\0';
		len_ = (len_ & ~is_large_flag) - 1 | (len_ & is_large_flag);
	}

	string string::substr(uint32_t pos, uint32_t size) const&
	{
		return {*this, pos, size};
	}

	string string::substr(uint32_t pos, uint32_t size) &&
	{
		return {static_cast<string&&>(*this), pos, size};
	}

	int32_t string::compare(string const& str) const
	{
		if ((len_ & ~is_large_flag) < str.size())
			return -1;
		else if ((len_ & ~is_large_flag) > str.size())
			return 1;
		else if ((len_ & ~is_large_flag) == str.size() && str.size() == 0)
			return 0;
		else
			return strcmp(data(), str.data());
	}

	int32_t string::compare(string_view str) const
	{
		if ((len_ & ~is_large_flag) < str.size())
			return -1;
		else if ((len_ & ~is_large_flag) > str.size())
			return 1;
		else if ((len_ & ~is_large_flag) == str.size() && str.size() == 0)
			return 0;
		else
			return strncmp(data(), str.data(), str.size());
	}

	int32_t string::compare(char const* str) const
	{
		uint32_t len = strlen(str);
		if ((len_ & ~is_large_flag) < len)
			return -1;
		else if ((len_ & ~is_large_flag) > len)
			return 1;
		else
			return strcmp(data(), str);
	}

	bool string::starts_with(string const& str) const
	{
		return strncmp(data(), str.data(), str.size()) == 0;
	}

	bool string::starts_with(string_view str) const
	{
		return strncmp(data(), str.data(), str.size()) == 0;
	}

	bool string::starts_with(char const* str) const
	{
		return strncmp(data(), str, strlen(str)) == 0;
	}

	bool string::starts_with(char c) const
	{
		return data()[0] == c;
	}

	bool string::ends_with(string const& str) const
	{
		return strncmp(data() + (len_ & ~is_large_flag) - str.size(), str.data(),
		               str.size()) == 0;
	}

	bool string::ends_with(string_view str) const
	{
		return strncmp(data() + (len_ & ~is_large_flag) - str.size(), str.data(),
		               str.size()) == 0;
	}

	bool string::ends_with(char const* str) const
	{
		uint32_t len = strlen(str);
		return strncmp(data() + (len_ & ~is_large_flag) - len, str, len) == 0;
	}

	bool string::ends_with(char c) const
	{
		return data()[(len_ & ~is_large_flag) - 1] == c;
	}

	bool string::contains(string const& str) const
	{
		if (str.size() > (len_ & ~is_large_flag) || str.size() == 0)
			return false;

		for (uint32_t i {0}, j {0}; i < len_; ++i)
		{
			if (len_ - i < str.size() - j)
				return false;

			if (data()[i] == str.data()[j])
			{
				++j;
				if (j == str.size())
					return true;
			}
			else
				j = 0;
		}
		return false;
	}

	bool string::contains(string_view str) const
	{
		if (str.size() > (len_ & ~is_large_flag) || str.size() == 0)
			return false;

		for (uint32_t i {0}, j {0}; i < (len_ & ~is_large_flag); ++i)
		{
			if ((len_ & ~is_large_flag) - i < str.size() - j)
				return false;

			if (data()[i] == str.data()[j])
			{
				++j;
				if (j == str.size())
					return true;
			}
			else
				j = 0;
		}
		return false;
	}

	bool string::contains(char const* str) const
	{
		uint32_t len = strlen(str);
		if (len > (len_ & ~is_large_flag) || len == 0)
			return false;

		for (uint32_t i {0}, j {0}; i < (len_ & ~is_large_flag); ++i)
		{
			if ((len_ & ~is_large_flag) - i < len - j)
				return false;

			if (data()[i] == str[j])
			{
				++j;
				if (j == len)
					return true;
			}
			else
				j = 0;
		}
		return false;
	}

	bool string::contains(char c) const
	{
		for (uint32_t i {0}; i < (len_ & ~is_large_flag); ++i)
			if (data()[i] == c)
				return true;
		return false;
	}

	uint32_t string::find(string const& str, uint32_t pos) const
	{
		if (str.size() > (len_ & ~is_large_flag) - pos || str.size() == 0)
			return UINT32_MAX;

		uint32_t match = 0;
		for (uint32_t i {pos}, j {0}; i < (len_ & ~is_large_flag); ++i)
		{
			if ((len_ & ~is_large_flag) - i < str.size() - j)
				return UINT32_MAX;

			if (data()[i] == str.data()[j])
			{
				if (j == 0)
					match = i;
				++j;
				if (j == str.size())
					return match;
			}
			else
				j = 0;
		}
		return UINT32_MAX;
	}

	uint32_t string::find(string_view str, uint32_t pos) const
	{
		if (str.size() > (len_ & ~is_large_flag) - pos || str.size() == 0)
			return UINT32_MAX;

		uint32_t match = 0;
		for (uint32_t i {pos}, j {0}; i < (len_ & ~is_large_flag); ++i)
		{
			if ((len_ & ~is_large_flag) - i < str.size() - j)
				return UINT32_MAX;

			if (data()[i] == str.data()[j])
			{
				if (j == 0)
					match = i;
				++j;
				if (j == str.size())
					return match;
			}
			else
				j = 0;
		}
		return UINT32_MAX;
	}

	uint32_t string::find(char const* str, uint32_t pos) const
	{
		uint32_t len = strlen(str);
		if (len > (len_ & ~is_large_flag) - pos || len == 0)
			return UINT32_MAX;

		uint32_t match = 0;
		for (uint32_t i {pos}, j {0}; i < (len_ & ~is_large_flag); ++i)
		{
			if ((len_ & ~is_large_flag) - i < len - j)
				return UINT32_MAX;

			if (data()[i] == str[j])
			{
				if (j == 0)
					match = i;
				++j;
				if (j == len)
					return match;
			}
			else
				j = 0;
		}
		return UINT32_MAX;
	}

	uint32_t string::find(char c, uint32_t pos) const
	{
		for (uint32_t i {pos}; i < (len_ & ~is_large_flag); ++i)
			if (data()[i] == c)
				return i;
		return UINT32_MAX;
	}

	uint32_t string::rfind(string const& str, uint32_t pos) const
	{
		if (pos == UINT32_MAX)
			pos = (len_ & ~is_large_flag) - 1;

		if (str.size() > pos || str.size() == 0)
			return UINT32_MAX;

		for (uint32_t i {pos + 1}, j {str.size()}; i > 0; --i)
		{
			if (i < j)
				return UINT32_MAX;

			if (data()[i - 1] == str.data()[j - 1])
			{
				--j;
				if (j == 0)
					return i - 1;
			}
			else
				j = str.size();
		}
		return UINT32_MAX;
	}

	uint32_t string::rfind(string_view str, uint32_t pos) const
	{
		if (pos == UINT32_MAX)
			pos = (len_ & ~is_large_flag) - 1;

		if (str.size() > pos || str.size() == 0)
			return UINT32_MAX;

		for (uint32_t i {pos + 1}, j {str.size()}; i > 0; --i)
		{
			if (i < j)
				return UINT32_MAX;

			if (data()[i - 1] == str.data()[j - 1])
			{
				--j;
				if (j == 0)
					return i - 1;
			}
			else
				j = str.size();
		}
		return UINT32_MAX;
	}

	uint32_t string::rfind(char const* str, uint32_t pos) const
	{
		uint32_t len = strlen(str);
		if (pos == UINT32_MAX)
			pos = (len_ & ~is_large_flag) - 1;

		if (len > pos || len == 0)
			return UINT32_MAX;

		for (uint32_t i {pos + 1}, j {len}; i > 0; --i)
		{
			if (i < j)
				return UINT32_MAX;

			if (data()[i - 1] == str[j - 1])
			{
				--j;
				if (j == 0)
					return i - 1;
			}
			else
				j = len;
		}
		return UINT32_MAX;
	}

	uint32_t string::rfind(char c, uint32_t pos) const
	{
		if (pos == UINT32_MAX)
			pos = (len_ & ~is_large_flag) - 1;

		for (uint32_t i {pos + 1}; i > 0; --i)
			if (data()[i - 1] == c)
				return i - 1;
		return UINT32_MAX;
	}

	bool string::is_large() const
	{
		return (len_ & is_large_flag) == is_large_flag;
	}
} // namespace mc
