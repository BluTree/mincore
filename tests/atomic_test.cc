#include "unit.hh"

#include <stdint.h>

#include <atomic.hh>

GROUP(atomic)
{
	struct s16
	{
		uint8_t a {2};
		uint8_t b {3};

		bool operator==(s16 const&) const = default;
	};

	TEST(load)
	{
		mc::atomic<int32_t> at0 {1};

		int32_t res0 = at0.load<mc::mem_order::seq_cst>();
		CHECK_EQ(res0, 1)

		res0 = at0.load<mc::mem_order::acquire>();
		CHECK_EQ(res0, 1)

		res0 = at0.load<mc::mem_order::relaxed>();
		CHECK_EQ(res0, 1)

		mc::atomic<s16> at1;

		s16 res1 = at1.load<mc::mem_order::seq_cst>();
		CHECK_EQ(res1, s16(2, 3))

		res1 = at1.load<mc::mem_order::acquire>();
		CHECK_EQ(res1, s16(2, 3))

		res1 = at1.load<mc::mem_order::relaxed>();
		CHECK_EQ(res1, s16(2, 3))
	}

	TEST(store)
	{
		mc::atomic<int32_t> at0 {1};

		at0.store<mc::mem_order::seq_cst>(2);
		int32_t res0 = at0.load<mc::mem_order::seq_cst>();
		CHECK_EQ(res0, 2)

		at0.store<mc::mem_order::release>(3);
		res0 = at0.load<mc::mem_order::acquire>();
		CHECK_EQ(res0, 3)

		at0.store<mc::mem_order::relaxed>(4);
		res0 = at0.load<mc::mem_order::relaxed>();
		CHECK_EQ(res0, 4)

		mc::atomic<s16> at1;

		at1.store<mc::mem_order::seq_cst>({3, 4});
		s16 res1 = at1.load<mc::mem_order::seq_cst>();
		CHECK_EQ(res1, s16(3, 4))

		at1.store<mc::mem_order::release>({4, 5});
		res1 = at1.load<mc::mem_order::acquire>();
		CHECK_EQ(res1, s16(4, 5))

		at1.store<mc::mem_order::relaxed>({5, 6});
		res1 = at1.load<mc::mem_order::relaxed>();
		CHECK_EQ(res1, s16(5, 6))
	}

	TEST(exchange)
	{
		mc::atomic<int32_t> at0 {1};

		int32_t res0 = at0.exchange(2);
		CHECK_EQ(res0, 1)
		res0 = at0.load();
		CHECK_EQ(res0, 2)

		mc::atomic<s16> at1;

		s16 res1 = at1.exchange({3, 4});
		CHECK_EQ(res1, s16(2, 3))
		res1 = at1.load();
		CHECK_EQ(res1, s16(3, 4))
	}

	TEST(compare_exchange)
	{
		mc::atomic<int32_t> at0 {1};

		int32_t exp0 = 1;
		bool    res0 = at0.compare_exchange(exp0, 2);
		CHECK_EQ(res0, true)
		int32_t val0 = at0.load();
		CHECK_EQ(val0, 2)

		mc::atomic<s16> at1;

		s16  exp1 = {2, 3};
		bool res1 = at1.compare_exchange(exp1, {3, 4});
		CHECK_EQ(res1, true)
		s16 val1 = at1.load();
		CHECK_EQ(val1, s16(3, 4))
	}

	TEST(fetch_add)
	{
		mc::atomic<int32_t> at0 {0};

		int32_t res0 = at0.fetch_add(2);
		CHECK_EQ(res0, 0)
		res0 = at0.fetch_add(5);
		CHECK_EQ(res0, 2)

		res0 = at0.load();
		CHECK_EQ(res0, 7)

		mc::atomic<uint32_t*> at1 {nullptr};

		uint32_t* res1 = at1.fetch_add(2);
		CHECK_EQ((uint64_t)res1, (uint64_t)nullptr);
		res1 = at1.fetch_add(5);
		CHECK_EQ((uint64_t)res1, (2 * sizeof(uint32_t)))

		res1 = at1.load();
		CHECK_EQ((uint64_t)res1, (7 * sizeof(uint32_t)))
	}

	TEST(fetch_sub)
	{
		mc::atomic<int32_t> at0 {0};

		int32_t res0 = at0.fetch_sub(2);
		CHECK_EQ(res0, 0)
		res0 = at0.fetch_sub(5);
		CHECK_EQ(res0, -2)

		res0 = at0.load();
		CHECK_EQ(res0, -7)

		mc::atomic<uint32_t*> at1 {(uint32_t*)(7 * sizeof(uint32_t))};

		uint32_t* res1 = at1.fetch_sub(2);
		CHECK_EQ((uint64_t)res1, (7 * sizeof(uint32_t)));
		res1 = at1.fetch_sub(5);
		CHECK_EQ((uint64_t)res1, (5 * sizeof(uint32_t)))

		res1 = at1.load();
		CHECK_EQ((uint64_t)res1, (uint64_t)nullptr)
	}

	TEST(fetch_and)
	{
		mc::atomic<int32_t> at0 {0x2 | 0x4 | 0xf};

		int32_t res0 = at0.fetch_and(0x2 | 0x4);
		CHECK_EQ(res0, 0x2 | 0x4 | 0xf)
		res0 = at0.fetch_and(0x2);
		CHECK_EQ(res0, 0x2 | 0x4)

		res0 = at0.load();
		CHECK_EQ(res0, 0x2)
	}

	TEST(fetch_xor)
	{
		mc::atomic<int32_t> at0 {0x2 | 0x4};

		int32_t res0 = at0.fetch_xor(0x4);
		CHECK_EQ(res0, 0x2 | 0x4)
		res0 = at0.fetch_xor(0x2 | 0x4);
		CHECK_EQ(res0, 0x2)

		res0 = at0.load();
		CHECK_EQ(res0, 0x4)
	}

	TEST(fetch_or)
	{
		mc::atomic<int32_t> at0 {0};

		int32_t res0 = at0.fetch_or(0x2);
		CHECK_EQ(res0, 0)
		res0 = at0.fetch_or(0x4 | 0xf);
		CHECK_EQ(res0, 0x2)

		res0 = at0.load();
		CHECK_EQ(res0, 0x2 | 0x4 | 0xf)
	}

	TEST(fetch_nand)
	{
		mc::atomic<int32_t> at0 {0};

		int32_t res0 = at0.fetch_nand(0x2);
		CHECK_EQ(res0, 0)
		res0 = at0.fetch_nand(0x4);
		CHECK_EQ(res0, (~(0 & 0x2)))

		res0 = at0.load();
		CHECK_EQ(res0, (~(-1 & (0x4))))
	}
}