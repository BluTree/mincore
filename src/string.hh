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
		string(string&& str, uint32_t pos = 0, uint32_t count = UINT32_MAX);
		string(std::initializer_list<char> ilist);
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

		void replace(uint32_t idx, uint32_t count, uint32_t count2, char c);
		// TODO replace first last
		void replace(uint32_t idx, uint32_t count, char const* str,
		             uint32_t count2 = UINT32_MAX);
		void replace(uint32_t idx, uint32_t count, string const& str, uint32_t pos = 0,
		             uint32_t count2 = UINT32_MAX);
		void replace(uint32_t idx, uint32_t count, string_view const& str,
		             uint32_t pos = 0, uint32_t count2 = UINT32_MAX);
		void replace(uint32_t idx, uint32_t count, std::initializer_list<char> ilist);

		void erase(uint32_t idx, uint32_t count = 1);

		void push_back(char c);
		void pop_back();

		string substr(uint32_t pos, uint32_t size = UINT32_MAX) const&;
		string substr(uint32_t pos, uint32_t size = UINT32_MAX) &&;

		int32_t compare(string const& str) const;
		int32_t compare(string_view str) const;
		int32_t compare(char const* str) const;

		bool starts_with(string const& str) const;
		bool starts_with(string_view str) const;
		bool starts_with(char const* str) const;
		bool starts_with(char c) const;

		bool ends_with(string const& str) const;
		bool ends_with(string_view str) const;
		bool ends_with(char const* str) const;
		bool ends_with(char c) const;

		bool contains(string const& str) const;
		bool contains(string_view str) const;
		bool contains(char const* str) const;
		bool contains(char c) const;

		uint32_t find(string const& str, uint32_t pos = 0) const;
		uint32_t find(string_view str, uint32_t pos = 0) const;
		uint32_t find(char const* str, uint32_t pos = 0) const;
		uint32_t find(char c, uint32_t pos = 0) const;

		uint32_t rfind(string const& str, uint32_t pos = UINT32_MAX) const;
		uint32_t rfind(string_view str, uint32_t pos = UINT32_MAX) const;
		uint32_t rfind(char const* str, uint32_t pos = UINT32_MAX) const;
		uint32_t rfind(char c, uint32_t pos = UINT32_MAX) const;

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
