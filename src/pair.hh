#pragma once

#include "compare.hh"

namespace mc
{
	template <typename T1, typename T2>
	struct pair
	{
		T1 first;
		T2 second;

		template <typename U1, typename U2>
		bool operator==(pair<U1, U2> const& other) const;
		template <typename U1, typename U2>
		common_comparison_t<decltype(declval<T1>() <=> declval<U1>()),
		                    decltype(declval<T2>() <=> declval<U2>())>
		operator<=>(pair<U1, U2> const& other) const;
	};
}

namespace mc
{
	template <typename T1, typename T2>
	template <typename U1, typename U2>
	bool pair<T1, T2>::operator==(pair<U1, U2> const& other) const
	{
		return first == other.first && second == other.second;
	}

	template <typename T1, typename T2>
	template <typename U1, typename U2>
	common_comparison_t<decltype(declval<T1>() <=> declval<U1>()),
	                    decltype(declval<T2>() <=> declval<U2>())>
	pair<T1, T2>::operator<=>(pair<U1, U2> const& other) const
	{
		auto res = first <=> other.first;
		if (res != 0)
			return res;
		else
			return second <=> other.second;
	}
}
