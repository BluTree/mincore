#pragma once

#include "alloc.hh" // Placement new definition

namespace mc
{
	template <typename T>
	concept is_complete = requires { sizeof(T); };

	template <typename From, typename To>
	concept convertible_to = __is_convertible_to(From, To);

	template <typename T1, typename T2>
	concept same_as = __is_same(T1, T2) && __is_same(T2, T1);

	template <typename T>
	concept destructible = requires(T a) {
		{ a.~T() };
	};

	template <typename T, typename... Args>
	concept constructible_from = destructible<T> && __is_constructible(T, Args...);

	template <typename To, typename From>
	concept assignable_from = requires(To lhs, From&& rhs) {
		{ lhs = static_cast<From &&>(rhs) } -> same_as<To>;
	};

	template <typename T>
	concept default_constructible =
		destructible<T> && constructible_from<T> && requires(T a) {
			{ T {} };
			{ new (static_cast<void*>(nullptr)) T };
		};

	template <typename T>
	concept move_constructible = constructible_from<T, T&&> && convertible_to<T&&, T>;

	template <typename T>
	concept copy_constructible =
		constructible_from<T, T&> && convertible_to<T&, T> &&
		constructible_from<T, T const&> && convertible_to<T const&, T> &&
		constructible_from<T, T const> && convertible_to<T const, T>;

	template <typename T>
	concept move_assignable = assignable_from<T&, T&&>;

	template <typename T>
	concept copy_assignable = assignable_from<T&, T&> && assignable_from<T&, T const&> &&
	                          assignable_from<T&, T const>;

	template <typename T>
	concept integral = __is_integral(T);

	template <typename T>
	concept pointer = __is_pointer(T);

	template <typename T>
	concept trivially_copyable = __is_trivially_copyable(T);

	template <typename T>
	concept enumeration = __is_enum(T);
}