#include "unit.hh"

#include <array.hh>
#include <string.hh>

GROUP(array)
{
	struct test_class
	{
		test_class(uint32_t i)
		: i_ {i}
		{}

		uint32_t i_;
	};

	TEST(ctor)
	{
		{
			mc::array<uint32_t, 5> arr0;
			CHECK_EQ(arr0.size(), 5);

			mc::array arr1 {0u, 1u, 2u, 3u, 4u};
			CHECK_EQ(arr1.size(), 5);
			for (uint32_t i {0}; i < arr1.size(); ++i)
				CHECK_EQ(arr1[i], i)
		}

		{
			mc::array<test_class, 5> arr1 {0, 1, 2, 3, 4};
			CHECK_EQ(arr1.size(), 5);
			for (uint32_t i {0}; i < arr1.size(); ++i)
				CHECK_EQ(arr1[i].i_, i)
		}
	}
}