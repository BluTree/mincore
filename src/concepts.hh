#pragma once

// TODO switch to usage of type traits if (and probably when) created
namespace mc
{
	template <typename From, typename To>
	concept convertible_to = __is_convertible_to(From, To);

	template <typename T>
	concept destructible = requires(T a) {
		{ a.~T() };
	};

	template <typename T, typename... Args>
	concept constructible_from = destructible<T> && __is_constructible(T, Args...);

	template <typename T>
	concept default_constructible =
		destructible<T> && constructible_from<T> && requires(T a) {
			{ T {} };
			{ new (static_cast<void*>(nullptr)) T };
		};

	template <typename T>
	concept copy_constructible =
		constructible_from<T, T&> && convertible_to<T&, T> &&
		constructible_from<T, T const&> && convertible_to<T const&, T> &&
		constructible_from<T, T const> && convertible_to<T const, T>;

	template <typename T>
	concept move_constructible = constructible_from<T, T&&> && convertible_to<T&&, T>;
}