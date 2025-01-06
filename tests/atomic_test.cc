#include "unit.hh"

#include <stdint.h>

#include <atomic.hh>

GROUP(atomic)
{
	struct s128
	{
		uint32_t a, b, c, d;
	};

	struct s256
	{
		s128 a, b;
	};

	struct s16

	{
		uint8_t a, b;
	};

	TEST(ctor)
	{
		mc::atomic<int32_t> at0 {1};
		at0.store(5);
		int32_t val0 = at0.load();
		CHECK_EQ(val0, 5);
		bool res = at0.compare_exchange(val0, 6);
		CHECK_EQ(res, true);
		val0 = at0.fetch_add(3);
		CHECK_EQ(val0, 6);

		[[maybe_unused]] uint8_t u;
		mc::atomic<s16>          at1({1, 2});
		CHECK_EQ(__atomic_always_lock_free(sizeof(s16), &at1.val_), true);
		at1.store({1, 0});
		s16 val1 = at1.load();
		CHECK_EQ(val1.a, 1);
		CHECK_EQ(val1.b, 0);
	}
}