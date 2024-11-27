#include "unit.hh"

#include <stdint.h>

#include <vector.hh>

GROUP(vector)
{
	class vec_test
	{
	public:
		int32_t val_ {-1};
	};

	class vec_test_copy
	{
	public:
		vec_test_copy(int32_t val)
		: val_ {val}
		{}

		vec_test_copy(vec_test_copy const& other)
		: val_ {other.val_}
		{}

		vec_test_copy(vec_test_copy&&) = delete;

		~vec_test_copy()
		{
			val_ = 0;
		}

		vec_test_copy& operator=(vec_test_copy const& other)
		{
			val_ = other.val_;
			return *this;
		}

		vec_test_copy& operator=(vec_test_copy&&) = delete;

		int32_t val_ {-1};
	};

	class vec_test_move
	{
	public:
		vec_test_move(int32_t val)
		: val_ {val}
		{}

		vec_test_move(vec_test_move const&) = delete;

		vec_test_move(vec_test_move&& other)
		: val_ {other.val_}
		{
			other.val_ = -2;
		}

		~vec_test_move()
		{
			val_ = 0;
		}

		vec_test_move& operator=(vec_test_move const&) = delete;

		vec_test_move& operator=(vec_test_move&& other)
		{
			val_ = other.val_;

			other.val_ = -2;
			return *this;
		}

		int32_t val_ {-1};
	};

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

	TEST(ctor_copy)
	{
		mc::vector<vec_test_copy> vec0(5, vec_test_copy(42));
		CHECK_EQ(vec0.size(), 5);
		CHECK_EQ(vec0.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec0.size()); ++i)
			CHECK_EQ(vec0[i].val_, 42)

		mc::vector<vec_test_copy> vec1 {0, 1, 2, 3, 4};
		CHECK_EQ(vec1.size(), 5);
		CHECK_EQ(vec1.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec1.size()); ++i)
			CHECK_EQ(vec1[i].val_, i)

		mc::vector<vec_test_copy> vec2(vec1);
		CHECK_EQ(vec2.size(), 5);
		CHECK_EQ(vec2.capacity(), 5);
		for (int32_t i {0}; i < static_cast<int32_t>(vec2.size()); ++i)
			CHECK_EQ(vec2[i].val_, i)

		mc::vector<vec_test_copy> vec3(static_cast<mc::vector<vec_test_copy>&&>(vec2));
		CHECK_EQ(vec3.size(), 5);
		CHECK_EQ(vec3.capacity(), 5);
		CHECK_EQ(vec2.size(), 0);
		CHECK_EQ(vec2.capacity(), 0);
		for (int32_t i {0}; i < static_cast<int32_t>(vec3.size()); ++i)
			CHECK_EQ(vec3[i].val_, i)
	}

	TEST(resize)
	{
		mc::vector<int32_t> vec0;
		CHECK_EQ(vec0.size(), 0);
		CHECK_EQ(vec0.capacity(), 0);
		vec0.resize(5);
		CHECK_EQ(vec0.size(), 5);

		mc::vector<vec_test> vec1;
		CHECK_EQ(vec1.size(), 0);
		vec1.resize(5);
		CHECK_EQ(vec1.size(), 5);
		for (uint32_t i {0}; i < vec1.size(); ++i)
			CHECK_EQ(vec1[i].val_, -1);

		mc::vector<int32_t> vec2;
		CHECK_EQ(vec2.size(), 0);
		CHECK_EQ(vec2.capacity(), 0);
		vec2.resize(5, 42);
		CHECK_EQ(vec2.size(), 5);
		for (uint32_t i {0}; i < vec2.size(); ++i)
			CHECK_EQ(vec2[i], 42);

		mc::vector<vec_test_copy> vec3;
		CHECK_EQ(vec3.size(), 0);
		vec3.resize(5, vec_test_copy(42));
		CHECK_EQ(vec3.size(), 5);
		for (uint32_t i {0}; i < vec3.size(); ++i)
			CHECK_EQ(vec3[i].val_, 42);
	}

	TEST(emplace_back) {}
}
