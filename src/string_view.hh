#pragma once

#include <stdint.h>

#include "alloc.hh"

using nullptr_t = decltype(nullptr);

namespace mc
{
	class string_view
	{
	public:
		string_view() = default;
		string_view(char const* str, uint32_t size = UINT32_MAX);
		string_view(nullptr_t) = delete;

		bool        empty() const;
		uint32_t    size() const;
		char const* data() const;

		char operator[](uint32_t pos) const;
		char front() const;
		char back() const;

		void remove_prefix(uint32_t size);
		void remove_suffix(uint32_t size);

		string_view substr(uint32_t pos, uint32_t size = UINT32_MAX);

		int32_t compare(string_view const& other);

		bool starts_with(string_view const& str);
		bool starts_with(char c);
		bool ends_with(string_view const& str);
		bool ends_with(char c);
		bool contains(string_view const& str);
		bool contains(char c);

		uint32_t find(string_view const& str, uint32_t pos = 0);
		uint32_t find(char c, uint32_t pos = 0);
		uint32_t rfind(string_view const& str, uint32_t pos = UINT32_MAX);
		uint32_t rfind(char c, uint32_t pos = UINT32_MAX);

	private:
		char const* str_ {nullptr};
		uint32_t    size_ {0};
	};
}