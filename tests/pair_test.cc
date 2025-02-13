#include "unit.hh"

#include <stdint.h>

#include <pair.hh>
#include <type_traits.hh>

GROUP(pair)
{
	TEST(comparison)
	{
		mc::pair p0 {1, 2};
		mc::pair p1 {1, 2};
		mc::pair p2 {2, 3};

		CHECK_EQ(p0, p0);
		CHECK_EQ(p0, p1);
		CHECK_NE(p1, p2);

		CHECK_EQ(p0 <=> p1, 0);
		CHECK_LT(p0 <=> p2, 0);
		CHECK_GT(p2 <=> p0, 0);
	}
}