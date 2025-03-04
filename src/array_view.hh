#pragma once

#include <stdint.h>

#include "initializer_list.hh"

using nullptr_t = decltype(nullptr);

namespace mc
{
	template <typename T>
	class array_view
	{
	public:
		constexpr array_view() = default;
		constexpr array_view(nullptr_t) = delete;
		constexpr array_view(nullptr_t, uint32_t) = delete;

		template <uint32_t S>
		constexpr array_view(T const (&arr)[S]);
		constexpr array_view(T const* data, uint32_t size);
		constexpr array_view(std::initializer_list<T> ilist);

		constexpr bool     empty() const;
		constexpr uint32_t size() const;
		constexpr T const* data() const;

		constexpr T const& operator[](uint32_t idx) const;
		constexpr T const& front() const;
		constexpr T const& back() const;

		constexpr array_view<T> subarr(uint32_t idx, uint32_t size);

	private:
		T const* arr_ {nullptr};
		uint32_t size_ {0};
	};
}

namespace mc
{
	template <typename T>
	template <uint32_t S>
	constexpr array_view<T>::array_view(T const (&arr)[S])
	: arr_ {arr}
	, size_ {S}
	{}

	template <typename T>
	constexpr array_view<T>::array_view(T const* data, uint32_t size)
	: arr_ {data}
	, size_ {size}
	{}

	template <typename T>
	constexpr array_view<T>::array_view(std::initializer_list<T> ilist)
	: arr_ {ilist.begin()}
	, size_ {static_cast<uint32_t>(ilist.size())}
	{}

	template <typename T>
	constexpr bool array_view<T>::empty() const
	{
		return size_ == 0;
	}

	template <typename T>
	constexpr uint32_t array_view<T>::size() const
	{
		return size_;
	}

	template <typename T>
	constexpr T const* array_view<T>::data() const
	{
		return arr_;
	}

	template <typename T>
	constexpr T const& array_view<T>::operator[](uint32_t idx) const
	{
		return arr_[idx];
	}

	template <typename T>
	constexpr T const& array_view<T>::front() const
	{
		return arr_[0];
	}

	template <typename T>
	constexpr T const& array_view<T>::back() const
	{
		return arr_[size_ - 1];
	}

	template <typename T>
	constexpr array_view<T> array_view<T>::subarr(uint32_t idx, uint32_t size)
	{
		return {arr_ + idx, size};
	}
}
