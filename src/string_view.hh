#pragma once

#include <stdint.h>

#include "compare.hh"

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

		char operator[](uint32_t idx) const;
		char front() const;
		char back() const;

		void remove_prefix(uint32_t size);
		void remove_suffix(uint32_t size);

		string_view substr(uint32_t idx, uint32_t size = UINT32_MAX) const;

		int32_t compare(string_view str) const;

		bool starts_with(string_view str) const;
		bool starts_with(char c) const;

		bool ends_with(string_view str) const;
		bool ends_with(char c) const;

		bool contains(string_view str) const;
		bool contains(char c) const;

		uint32_t find(string_view str, uint32_t idx = 0) const;
		uint32_t find(char c, uint32_t idx = 0) const;
		uint32_t rfind(string_view str, uint32_t idx = UINT32_MAX) const;
		uint32_t rfind(char c, uint32_t idx = UINT32_MAX) const;

		bool operator==(string_view str) const;

		std::strong_ordering operator<=>(string_view str) const;

	private:
		char const* str_ {nullptr};
		uint32_t    len_ {0};
	};
}