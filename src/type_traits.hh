#pragma once

namespace mc
{
	template <bool Cond, typename True, typename False>
	struct conditional
	{
		using type = True;
	};

	template <typename True, typename False>
	struct conditional<false, True, False>
	{
		using type = False;
	};

	template <bool Cond, typename True, typename False>
	using conditional_t = conditional<Cond, True, False>::type;

	template <typename T>
	struct remove_pointer
	{
		using type = T;
	};

	template <typename T>
	struct remove_pointer<T*>
	{
		using type = T;
	};

	template <typename T>
	struct remove_pointer<T* const>
	{
		using type = T;
	};

	template <typename T>
	using remove_pointer_t = remove_pointer<T>::type;

	template <typename T>
	struct add_lvalue
	{
		using type = T&;
	};

	template <>
	struct add_lvalue<void>
	{
		using type = void;
	};

	template <typename T>
	using add_lvalue_t = add_lvalue<T>::type;

	template <typename T>
	struct add_rvalue
	{
		using type = T&&;
	};

	template <>
	struct add_rvalue<void>
	{
		using type = void;
	};

	template <typename T>
	using add_rvalue_t = add_rvalue<T>::type;

	template <typename T>
	consteval add_rvalue_t<T> declval()
	{
		static_assert(false, "declval not allowed in an evaluated context");
	}
}
