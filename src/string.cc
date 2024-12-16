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

	string::string(string&& other)
	: len_ {other.len_}
	{
		if (is_large())
		{
			large_ = other.large_;
			other.large_.str_ = nullptr;
			other.large_.cap_ = 0;
		}
		else
			small_ = other.small_;

		other.len_ = 0;
	}

	string::string(string const& other, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = other.size();
		else
			len_ = count;

		len_ &= ~is_large_flag;

		if (other.is_large())
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, other.large_.str_ + pos, len_);
				small_.str_[len_] = '\0';
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, other.large_.str_ + pos, len_);
				large_.str_[len_] = '\0';
				len_ |= is_large_flag;
			}
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_);
			small_.str_[len_] = '\0';
		}
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

	string& string::operator=(char const* str)
	{
		uint32_t str_len = static_cast<uint32_t>(strlen(str)) & ~is_large_flag;

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
			memcpy(large_.str_, str, str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else if (str_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(str_len + 1, alignof(char)));

			large_.cap_ = str_len;
			large_.str_ = new_str;
			memcpy(large_.str_, str, str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, str, str_len);
			small_.str_[str_len] = '\0';
			len_ = str_len;
		}

		return *this;
	}

	string& string::operator=(string_view str)
	{
		uint32_t str_len = str.size() & ~is_large_flag;

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
			memcpy(large_.str_, str.data(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else if (str_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(str_len + 1, alignof(char)));

			large_.cap_ = str_len;
			large_.str_ = new_str;
			memcpy(large_.str_, str.data(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, str.data(), str_len);
			small_.str_[str_len] = '\0';
			len_ = str_len;
		}

		return *this;
	}

	string& string::operator=(string const& other)
	{
		uint32_t str_len = other.len_ & ~is_large_flag;

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
			memcpy(large_.str_, other.data(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else if (str_len >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(str_len + 1, alignof(char)));

			large_.cap_ = str_len;
			large_.str_ = new_str;
			memcpy(large_.str_, other.data(), str_len);
			large_.str_[str_len] = '\0';
			len_ = str_len | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, other.data(), str_len);
			small_.str_[str_len] = '\0';
			len_ = str_len;
		}

		return *this;
	}

	string& string::operator=(string&& other)
	{
		if (is_large())
			free(large_.str_, large_.cap_ + 1, alignof(char));

		len_ = other.len_;
		if (is_large())
		{
			large_.str_ = other.large_.str_;
			large_.cap_ = other.large_.cap_;
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_ + 1);
		}

		other.len_ = 0;
		return *this;
	}

	string& string::operator=(std::initializer_list<char> ilist)
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

		return *this;
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

	char& string::operator[](uint32_t pos) &
	{
		char* str = is_large() ? large_.str_ : small_.str_;
		return str[pos];
	}

	char const& string::operator[](uint32_t pos) const&
	{
		char const* str = is_large() ? large_.str_ : small_.str_;
		return str[pos];
	}

	char& string::front() &
	{
		char* str = is_large() ? large_.str_ : small_.str_;
		return *str;
	}

	char const& string::front() const&
	{
		char const* str = is_large() ? large_.str_ : small_.str_;
		return *str;
	}

	char& string::back() &
	{
		char* str = is_large() ? large_.str_ : small_.str_;
		return str[len_ & ~is_large_flag - 1];
	}

	char const& string::back() const&
	{
		char const* str = is_large() ? large_.str_ : small_.str_;
		return str[len_ & ~is_large_flag - 1];
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
			count = str.size();

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

	void string::assign(string const& other, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			count = other.size();

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
			memcpy(large_.str_, other.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else if (count >= small_size)
		{
			char* new_str = reinterpret_cast<char*>(alloc(count + 1, alignof(char)));

			large_.cap_ = count;
			large_.str_ = new_str;
			memcpy(large_.str_, other.data() + pos, count);
			large_.str_[count] = '\0';
			len_ = count | is_large_flag;
		}
		else
		{
			memcpy(small_.str_, other.data() + pos, count);
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

	void string::assign(string&& other)
	{
		if (is_large())
			free(large_.str_, large_.cap_ + 1, alignof(char));

		len_ = other.len_;
		if (is_large())
		{
			large_.str_ = other.large_.str_;
			large_.cap_ = other.large_.cap_;
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_ + 1);
		}

		other.len_ = 0;
	}

	bool string::is_large() const
	{
		return (len_ & is_large_flag) == is_large_flag;
	}
} // namespace mc
