#include "string_view.hh"

#include <string.h>

namespace mc
{
	string_view::string_view(char const* str, uint32_t size)
	: str_ {str}
	{
		if (size == UINT32_MAX)
			len_ = strlen(str);
		else
			len_ = size;
	}

	bool string_view::empty() const
	{
		return len_ == 0;
	}

	uint32_t string_view::size() const
	{
		return len_;
	}

	char const* string_view::data() const
	{
		return str_;
	}

	char string_view::operator[](uint32_t pos) const
	{
		return str_[pos];
	}

	char string_view::front() const
	{
		return str_[0];
	}

	char string_view::back() const
	{
		return str_[len_ - 1];
	}

	void string_view::remove_prefix(uint32_t size)
	{
		str_ += size;
		len_ -= size;
	}

	void string_view::remove_suffix(uint32_t size)
	{
		len_ -= size;
	}

	string_view string_view::substr(uint32_t pos, uint32_t size) const
	{
		return {str_ + pos, size};
	}

	int32_t string_view::compare(string_view str) const
	{
		if (len_ < str.len_)
			return -1;
		else if (len_ > str.len_)
			return 1;
		else
			return strncmp(str_, str.str_, len_);
	}

	bool string_view::starts_with(string_view str) const
	{
		return strncmp(str_, str.str_, str.len_) == 0;
	}

	bool string_view::starts_with(char c) const
	{
		return str_[0] == c;
	}

	bool string_view::ends_with(string_view str) const
	{
		return strncmp(str_ + len_ - str.len_, str.str_, str.len_) == 0;
	}

	bool string_view::ends_with(char c) const
	{
		return str_[len_ - 1] == c;
	}

	bool string_view::contains(string_view str) const
	{
		if (str.len_ > len_ || str.len_ == 0)
			return false;

		for (uint32_t i {0}, j {0}; i < len_; ++i)
		{
			if (len_ - i < str.len_ - j)
				return false;

			if (str_[i] == str.str_[j])
			{
				++j;
				if (j == str.len_)
					return true;
			}
			else
				j = 0;
		}
		return false;
	}

	bool string_view::contains(char c) const
	{
		for (uint32_t i {0}; i < len_; ++i)
			if (str_[i] == c)
				return true;
		return false;
	}

	uint32_t string_view::find(string_view str, uint32_t pos) const
	{
		if (str.len_ > len_ - pos || str.len_ == 0)
			return UINT32_MAX;

		uint32_t match = 0;
		for (uint32_t i {pos}, j {0}; i < len_; ++i)
		{
			if (len_ - i < str.len_ - j)
				return UINT32_MAX;

			if (str_[i] == str.str_[j])
			{
				if (j == 0)
					match = i;
				++j;
				if (j == str.len_)
					return match;
			}
			else
				j = 0;
		}
		return UINT32_MAX;
	}

	uint32_t string_view::find(char c, uint32_t pos) const
	{
		for (uint32_t i {pos}; i < len_; ++i)
			if (str_[i] == c)
				return i;
		return UINT32_MAX;
	}

	uint32_t string_view::rfind(string_view str, uint32_t pos) const
	{
		if (pos == UINT32_MAX)
			pos = len_ - 1;

		if (str.len_ > pos || str.len_ == 0)
			return UINT32_MAX;

		for (uint32_t i {pos + 1}, j {str.len_}; i > 0; --i)
		{
			if (i < j)
				return UINT32_MAX;

			if (str_[i - 1] == str.str_[j - 1])
			{
				--j;
				if (j == 0)
					return i - 1;
			}
			else
				j = str.len_;
		}
		return UINT32_MAX;
	}

	uint32_t string_view::rfind(char c, uint32_t pos) const
	{
		if (pos == UINT32_MAX)
			pos = len_ - 1;

		for (uint32_t i {pos + 1}; i > 0; --i)
			if (str_[i - 1] == c)
				return i - 1;
		return UINT32_MAX;
	}
}