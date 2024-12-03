#pragma once

#include <stdint.h>

#include "initializer_list.hh"
#include "string_view.hh"

namespace mc
{
	class string
	{
	public:
		string();
		string(uint32_t count, char c);
		// TODO ctor first last
		string(char* str, uint32_t count = UINT32_MAX);
		string(string_view str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(string const& other);
		string(string&& other);
		string(string const& other, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(std::initializer_list<char> ilist);

		bool        empty() const;
		uint32_t    size() const;
		uint32_t    capacity() const;
		char*       data() &;
		char const* data() const&;

		char&       operator[](uint32_t pos) &;
		char const& operator[](uint32_t pos) const&;
		char&       front() &;
		char const& front() const&;
		char&       back() &;
		char const& back() const&;

	private:
		static constexpr uint32_t small_size = 12;

		union
		{
			struct
			{
				char*    str_ {nullptr};
				uint32_t cap_ {0};
			} large_;

			struct
			{
				char str_[small_size] {0};

			} small_;
		};

		uint32_t len_ : 31 {0};
		bool     is_large_ : 1 {false};
	};
}
