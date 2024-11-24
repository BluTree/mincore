#include "unit.hh"

#include <vector.hh>

GROUP(vector)
{
	TEST(ctor)
	{
		mc::vector<int32_t> vec0;
		CHECK_EQ(vec0.size(), 0);
		CHECK_EQ(vec0.capacity(), 0);

		mc::vector<int32_t> vec1(5);
		CHECK_EQ(vec1.size(), 5);
		CHECK_EQ(vec1.capacity(), 5);

		mc::vector<int32_t> vec2(5, 42);
		CHECK_EQ(vec2.size(), 5);
		CHECK_EQ(vec2.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec2.size()); ++i)
			CHECK_EQ(vec2[i], 42)

		mc::vector<int32_t> vec3 {0, 1, 2, 3, 4};
		CHECK_EQ(vec3.size(), 5);
		CHECK_EQ(vec3.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec3.size()); ++i)
			CHECK_EQ(vec3[i], i)

		mc::vector<int32_t> vec4(vec3);
		CHECK_EQ(vec4.size(), 5);
		CHECK_EQ(vec4.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec4.size()); ++i)
			CHECK_EQ(vec4[i], i)

		mc::vector<int32_t> vec5(static_cast<mc::vector<int32_t>&&>(vec4));
		CHECK_EQ(vec5.size(), 5);
		CHECK_EQ(vec5.capacity(), 5);
		CHECK_EQ(vec4.size(), 0);
		CHECK_EQ(vec4.capacity(), 0);
		for (int32_t i {0}; i < static_cast<int32_t>(vec5.size()); ++i)
			CHECK_EQ(vec5[i], i)
	}

	TEST(insert) {}
}
