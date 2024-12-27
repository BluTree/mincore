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
		string(string const& str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(std::initializer_list<char> ilist);
		string(string&& str);
		~string();

		bool        empty() const;
		uint32_t    size() const;
		uint32_t    capacity() const;
		char*       data() &;
		char const* data() const&;

		operator string_view() const;

		char&       operator[](uint32_t pos) &;
		char const& operator[](uint32_t pos) const&;
		char&       front() &;
		char const& front() const&;
		char&       back() &;
		char const& back() const&;

		void clear();
		void reserve(uint32_t cap);
		void fit();

		void resize(uint32_t size, char c = '\0');

		void assign(uint32_t count, char c);
		// TODO assign first last
		void assign(char const* str, uint32_t count = UINT32_MAX);
		void assign(string_view str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		void assign(string const& str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		void assign(std::initializer_list<char> ilist);
		void assign(string&& str);

		string& operator=(char const* str);
		string& operator=(string_view str);
		string& operator=(string const& str);
		string& operator=(string&& str);
		string& operator=(std::initializer_list<char> ilist);

		void insert(uint32_t idx, uint32_t count, char c);
		// TODO insert first last
		void insert(uint32_t idx, char const* str, uint32_t count = UINT32_MAX);
		void insert(uint32_t idx, string const& str, uint32_t pos = 0,
		            uint32_t count = UINT32_MAX);
		void insert(uint32_t idx, string_view const& str, uint32_t pos = 0,
		            uint32_t count = UINT32_MAX);
		void insert(uint32_t idx, std::initializer_list<char> ilist);

		void append(uint32_t count, char c);
		// TODO append first last
		void append(char const* str, uint32_t count = UINT32_MAX);
		void append(string const& str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		void append(string_view const& str, uint32_t pos = 0,
		            uint32_t count = UINT32_MAX);
		void append(std::initializer_list<char> ilist);

		string& operator+=(char const* str);
		string& operator+=(string_view str);
		string& operator+=(string const& str);
		string& operator+=(std::initializer_list<char> ilist);

		void erase(uint32_t idx, uint32_t count = 1);

		void push_back(char c);
		void pop_back();

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
