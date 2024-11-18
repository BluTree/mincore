#pragma once

#include <stdint.h>

#define GROUP(Name)                                                                      \
	namespace namesp_group_##Name                                                        \
	{                                                                                    \
		struct group_##Name : public group_base                                          \
		{                                                                                \
			group_##Name()                                                               \
			: group_base(#Name)                                                          \
			{}                                                                           \
		};                                                                               \
		group_##Name group_instance;                                                     \
	}                                                                                    \
	namespace namesp_group_##Name

#define TEST(Name)                                                                       \
	struct test_##Name : public test_base                                                \
	{                                                                                    \
		test_##Name(group_base* group)                                                   \
		: test_base(#Name, group)                                                        \
		{}                                                                               \
		void run() override;                                                             \
	};                                                                                   \
	test_##Name test_##Name_instance(&group_instance);                                   \
	void        test_##Name::run()

#define CHECK_EQ(a, b)                                                                   \
	if (a != b)                                                                          \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, "==", __LINE__, __FILE__);                             \
	}                                                                                    \
	else                                                                                 \
		++checks_cnt;

#define CHECK_NE(a, b)                                                                   \
	if (a == b)                                                                          \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, "!=", __LINE__, __FILE__);                             \
	}                                                                                    \
	else                                                                                 \
		++checks_cnt

#define CHECK_GT(a, b)                                                                   \
	if (a > b)                                                                           \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, ">", __LINE__, __FILE__);                              \
	}                                                                                    \
	else                                                                                 \
		++checks_cnt;

#define CHECK_GE(a, b)                                                                   \
	if (a >= b)                                                                          \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, ">=", __LINE__, __FILE__);                             \
	}                                                                                    \
	else                                                                                 \
	{                                                                                    \
		++checks_cnt;                                                                    \
	}

#define CHECK_LT(a, b)                                                                   \
	if (a < b)                                                                           \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, "<", __LINE__, __FILE__);                              \
	}                                                                                    \
	else                                                                                 \
		++checks_cnt

#define CHECK_LE(a, b)                                                                   \
	if (a <= b)                                                                          \
	{                                                                                    \
		++checks_fail_cnt;                                                               \
		++checks_cnt;                                                                    \
		print_check_error(#a, #b, "<=", __LINE__, __FILE__);                             \
	}                                                                                    \
	else                                                                                 \
		++checks_cnt;

struct group_base;
struct test_base;

struct unit_state
{
	static unit_state& instance();

	~unit_state();

	int run_tests();

	void add_group(group_base* group);

	group_base** groups {nullptr};
	uint32_t     group_size {0};
	uint32_t     group_capa {0};
};

struct group_base
{
	group_base(char const* name);
	virtual ~group_base();

	void run();

	void add_test(test_base* test);

	char const* name;
	test_base** tests {nullptr};
	uint32_t    test_size {0};
	uint32_t    test_capa {0};

	uint32_t tests_fail_cnt {0};
};

struct test_base
{
	test_base(char const* name, group_base* group)
	: name {name}
	{
		group->add_test(this);
	}

	virtual void run() = 0;

	char const* name;
	uint32_t    checks_cnt {0};
	uint32_t    checks_fail_cnt {0};
};

void print_check_error(
	char const* a, char const* b, char const* op, int line, char const* file);