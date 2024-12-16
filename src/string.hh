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
		string(char const* str, uint32_t count = UINT32_MAX);
		string(string_view str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(string const& other, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(std::initializer_list<char> ilist);
		string(string&& other);
		~string();

		string& operator=(char const* str);
		string& operator=(string_view str);
		string& operator=(string const& other);
		string& operator=(string&& other);
		string& operator=(std::initializer_list<char> ilist);

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

		void clear();
		void reserve(uint32_t cap);
		void fit();

		void assign(uint32_t count, char c);
		// TODO assign first last
		void assign(char const* str, uint32_t count = UINT32_MAX);
		void assign(string_view str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		void assign(string const& other, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		void assign(std::initializer_list<char> ilist);
		void assign(string&& other);

	private:
		static constexpr uint32_t small_size = 16;
		static constexpr uint32_t is_large_flag = 0x8000'0000;

		bool is_large() const;

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

		uint32_t len_ {0};
	};
}
