#pragma once

namespace mc
{
	template <typename T1, typename T2>
	bool is_same_v = __is_same(T1, T2);
}
