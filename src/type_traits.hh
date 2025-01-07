#pragma once

namespace mc
{
	template <typename T1, typename T2>
	constexpr bool is_same_v = __is_same(T1, T2);

	template <typename T>
	constexpr bool is_integral_v = __is_integral(T);

	template <typename T>
	constexpr bool is_pointer_v = __is_pointer(T);

	template <typename T>
	constexpr bool is_trivially_copyable_v = __is_trivially_copyable(T);

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
}
