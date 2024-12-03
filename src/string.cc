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

		if (len_ < small_size)
		{
			memcpy(small_.str_, str, len_);
			small_.str_[len_ + 1] = '\0';
			is_large_ = false;
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str, len_);
			large_.str_[len_ + 1] = '\0';
			is_large_ = true;
		}
	}

	string::string(string_view str, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = str.size() - pos;
		else
			len_ = count - pos;

		if (len_ < small_size)
		{
			memcpy(small_.str_, str.data() + pos, len_);
			small_.str_[len_ + 1] = '\0';
			is_large_ = false;
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, str.data() + pos, len_);
			large_.str_[len_ + 1] = '\0';
			is_large_ = true;
		}
	}

	string::string(string const& other)
	: len_ {other.len_}
	{
		if (other.is_large_)
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, other.large_.str_, len_);
				small_.str_[len_ + 1] = '\0';
				is_large_ = false;
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, other.large_.str_, len_);
				large_.str_[len_ + 1] = '\0';
				is_large_ = true;
			}
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_);
			small_.str_[len_ + 1] = '\0';
			is_large_ = false;
		}
	}

	string::string(string&& other)
	: len_ {other.len_}
	, is_large_ {other.is_large_}
	{
		if (is_large_)
		{
			large_ = other.large_;
			other.large_.str_ = nullptr;
			other.large_.cap_ = 0;
		}
		else
			small_ = other.small_;

		other.len_ = 0;
		other.is_large_ = false;
	}

	string::string(string const& other, uint32_t pos, uint32_t count)
	{
		if (count == UINT32_MAX)
			len_ = other.size() - pos;
		else
			len_ = count - pos;

		if (other.is_large_)
		{
			if (len_ < small_size)
			{
				memcpy(small_.str_, other.large_.str_, len_);
				small_.str_[len_ + 1] = '\0';
				is_large_ = false;
			}
			else
			{
				large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
				large_.cap_ = len_;

				memcpy(large_.str_, other.large_.str_, len_);
				large_.str_[len_ + 1] = '\0';
				is_large_ = true;
			}
		}
		else
		{
			memcpy(small_.str_, other.small_.str_, len_);
			small_.str_[len_ + 1] = '\0';
			is_large_ = false;
		}
	}

	string::string(std::initializer_list<char> ilist)
	: len_ {static_cast<uint32_t>(ilist.size())}
	{
		if (len_ < small_size)
		{
			memcpy(small_.str_, ilist.begin(), len_);
			small_.str_[len_ + 1] = '\0';
			is_large_ = false;
		}
		else
		{
			large_.str_ = reinterpret_cast<char*>(alloc(len_ + 1, alignof(char)));
			large_.cap_ = len_;

			memcpy(large_.str_, ilist.begin(), len_);
			large_.str_[len_ + 1] = '\0';
			is_large_ = true;
		}
	}

	bool string::empty() const
	{
		return len_ == 0;
	}

	uint32_t string::size() const
	{
		return len_;
	}

	uint32_t string::capacity() const
	{
		return is_large_ ? large_.cap_ : small_size;
	}

	char* string::data() &
	{
		return is_large_ ? large_.str_ : small_.str_;
	}

	char const* string::data() const&
	{
		return is_large_ ? large_.str_ : small_.str_;
	}

	char& string::operator[](uint32_t pos) &
	{
		char* buf = is_large_ ? large_.str_ : small_.str_;
		return buf[pos];
	}

	char const& string::operator[](uint32_t pos) const&
	{
		char const* buf = is_large_ ? large_.str_ : small_.str_;
		return buf[pos];
	}

	char& string::front() &
	{
		char* buf = is_large_ ? large_.str_ : small_.str_;
		return *buf;
	}

	char const& string::front() const&
	{
		char const* buf = is_large_ ? large_.str_ : small_.str_;
		return *buf;
	}

	char& string::back() &
	{
		char* buf = is_large_ ? large_.str_ : small_.str_;
		return buf[len_ - 1];
	}

	char const& string::back() const&
	{
		char const* buf = is_large_ ? large_.str_ : small_.str_;
		return buf[len_ - 1];
	}
}
