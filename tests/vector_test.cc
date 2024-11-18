#include "unit.hh"

#include <vector.hh>

GROUP(vector)
{
	TEST(init)
	{
		mc::vector<int> vec(5);
		vec[0] = 0;

		CHECK_EQ(vec[0], 0)
		CHECK_EQ(vec[1], 0)

		mc::vector<int> vec2 {0, 1, 2, 3, 4};
		// for (int i {0}; i < 5; ++i)
		// 	CHECK_EQ(vec[i], i)
	}
}
