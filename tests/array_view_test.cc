#include "unit.hh"

#include <array_view.hh>

GROUP(array_view)
{
	bool ctor_ilist(mc::array_view<uint32_t> av)
	{
		bool res {true};
		res &= av.size() == 5;
		for (uint32_t i {0}; i < av.size(); ++i)
			res &= av[i] == i;

		return res;
	}

	TEST(ctor)
	{
		uint32_t       arr[5] {0, 1, 2, 3, 4};
		mc::array_view av0(arr);
		CHECK_EQ(av0.size(), 5);
		for (uint32_t i {0}; i < av0.size(); ++i)
			CHECK_EQ(av0[i], i)

		uint32_t*      arr_heap = new uint32_t[5] {0, 1, 2, 3, 4};
		mc::array_view av1 {arr_heap, 5};
		CHECK_EQ(av1.size(), 5);
		for (uint32_t i {0}; i < av1.size(); ++i)
			CHECK_EQ(av1[i], i)

		delete[] arr_heap;

		ctor_ilist({0, 1, 2, 3, 4});
	}

	TEST(subarr)
	{
		uint32_t       arr[5] {0, 1, 2, 3, 4};
		mc::array_view av0(arr);
		CHECK_EQ(av0.size(), 5);

		mc::array_view av1 = av0.subarr(2, 3);
		CHECK_EQ(av1.size(), 3);
		for (uint32_t i {2}; i < 5; ++i)
			CHECK_EQ(av1[i - 2], i);
	}
}