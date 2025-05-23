#pragma once

#include <stdint.h>

#include "array_view.hh"
#include "concepts.hh"

namespace mc
{
	template <typename T, uint32_t S>
		requires(S > 0)
	class array
	{
	public:
		constexpr uint32_t size() const;
		constexpr T*       data() &;
		constexpr T const* data() const&;

		constexpr operator array_view<T>() const;

		constexpr T&       operator[](uint32_t idx) &;
		constexpr T const& operator[](uint32_t idx) const&;
		constexpr T&       front() &;
		constexpr T const& front() const&;
		constexpr T&       back() &;
		constexpr T const& back() const&;

		// arr_ is public because there isn't any possibility for a user defined
		// constructor to behave like an array initialization (that is, without calling
		// extra constructors for the final object, either it is by copy or by move).
		// arr_ can be used directly, but it isn't designed to.
		T arr_[S];
	};

	template <typename T, typename... U>
	array(T, U...) -> array<T, 1 + sizeof...(U)>;
}

namespace mc
{

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr uint32_t array<T, S>::size() const
	{
		return S;
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T* array<T, S>::data() &
	{
		return arr_;
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T const* array<T, S>::data() const&
	{
		return arr_;
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr array<T, S>::operator array_view<T>() const
	{
		return {arr_, S};
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T& array<T, S>::operator[](uint32_t idx) &
	{
		return arr_[idx];
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T const& array<T, S>::operator[](uint32_t idx) const&
	{
		return arr_[idx];
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T& array<T, S>::front() &
	{
		return arr_[0];
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T const& array<T, S>::front() const&
	{
		return arr_[0];
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T& array<T, S>::back() &
	{
		return arr_[S - 1];
	}

	template <typename T, uint32_t S>
		requires(S > 0)
	constexpr T const& array<T, S>::back() const&
	{
		return arr_[S - 1];
	}
};