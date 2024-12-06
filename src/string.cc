#include "string.hh"

#include "alloc.hh"

#include <string.h>

namespace mc
{
	// The user-defined ctor is needed because clang and gcc (< 13) misunderstand the
	// union not being trivially constructable.
	// See https://github.com/llvm/llvm-project/pull/82407 for the clang fix.
	string::string()
	: small_ {0}
	{}

	string::string(char* str, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = strlen(str);
		else
			len_ = count;

		len_ &= ~is_large_flag;

		if (len_ < small_size)
		{
			memcpy(small_.str_, str, len_);
			small_.str_[len_ + 1] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str, len_);
			large_.str_[len_ + 1] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::string(string_view str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = str.size() - pos;
		else
			len_ = count - pos;

		len_ &= ~is_large_flag;

		if (len_ < small_size)
		{
			memcpy(small_.str_, str.data() + pos, len_);
			small_.str_[len_ + 1] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str.data() + pos, len_);
			large_.str_[len_ + 1] = '\0';
			len_ |= is_large_flag;
		}
	}

	string::string(string const& other)
	: len_ {other.len_}
	{
		len_ &= ~is_large_flag;
		if (other.is_large())
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, other.large_.str_, len_);
				small_.str_[len_ + 1] = '\0';
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, other.large_.str_, len_);
				large_.str_[len_ + 1] = '\0';
				len_ |= is_large_flag;
			}
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_);
			small_.str_[len_ + 1] = '\0';
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
			len_ = other.size() - pos;
		else
			len_ = count - pos;

		len_ &= ~is_large_flag;

		if (other.is_large())
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, other.large_.str_, len_);
				small_.str_[len_ + 1] = '\0';
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, other.large_.str_, len_);
				large_.str_[len_ + 1] = '\0';
				len_ |= is_large_flag;
			}
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_);
			small_.str_[len_ + 1] = '\0';
		}
	}

	string::string(std::initializer_list<char> ilist)
	: len_ {static_cast<uint32_t>(ilist.size())}
	{
		if (len_ < small_size)
		{
			memcpy(small_.str_, ilist.begin(), len_);
			small_.str_[len_ + 1] = '\0';
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, ilist.begin(), len_);
			large_.str_[len_ + 1] = '\0';
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
		return is_large() ? large_.cap_ : small_size;
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

	bool string::is_large() const
	{
		return (len_ & is_large_flag) == is_large_flag;
	}
} // namespace mc
