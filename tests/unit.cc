#include "unit.hh"

#include <stdio.h>
#include <stdlib.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define DEFAULT "\x1b[0m"

unit_state& unit_state::instance()
{
	static unit_state inst;
	return inst;
}

unit_state::~unit_state()
{
	if (groups)
		free(groups);
}

int unit_state::run_tests()
{
	bool success = true;
	for (uint32_t i {0}; i < group_size; ++i)
	{
		printf("[%s]\n", groups[i]->name);
		groups[i]->run();
		if (groups[i]->tests_fail_cnt)
		{
			success = false;
			printf("[" RED "failed" DEFAULT "] (%d/%d)\n",
			       groups[i]->test_size - groups[i]->tests_fail_cnt,
			       groups[i]->test_size);
		}
		else
			printf("[" GREEN "passed" DEFAULT "]   (%d/%d)\n", groups[i]->test_size,
			       groups[i]->test_size);
	}

	return success;
}

void unit_state::add_group(group_base* group)
{
	if (!group_capa)
	{
		groups = reinterpret_cast<group_base**>(malloc(sizeof(group_base*)));
		group_capa = 1;
	}
	else if (group_size == group_capa)
	{
		group_capa *= 2;
		groups = reinterpret_cast<group_base**>(
			realloc(groups, sizeof(group_base*) * group_capa));
	}

	groups[group_size] = group;
	++group_size;
}

group_base::group_base(char const* name)
: name {name}
{
	unit_state::instance().add_group(this);
}

group_base::~group_base()
{
	if (tests)
		free(tests);
}

void group_base::run()
{
	for (uint32_t i {0}; i < test_size; ++i)
	{
		printf("|-[%s]\n", tests[i]->name);
		tests[i]->run();
		if (tests[i]->checks_fail_cnt)
		{
			printf("|-[" RED "failed" DEFAULT "] (%d/%d)\n",
			       tests[i]->checks_cnt - tests[i]->checks_fail_cnt,
			       tests[i]->checks_cnt);
			++tests_fail_cnt;
		}
		else
			printf("|-[" GREEN "passed" DEFAULT "] (%d/%d)\n", tests[i]->checks_cnt,
			       tests[i]->checks_cnt);
	}
}

void group_base::add_test(test_base* test)
{
	if (!test_capa)
	{
		tests = reinterpret_cast<test_base**>(malloc(sizeof(test_base*)));
		test_capa = 1;
	}
	else if (test_size == test_capa)
	{
		test_capa *= 2;
		tests =
			reinterpret_cast<test_base**>(realloc(tests, sizeof(test_base*) * test_capa));
	}

	tests[test_size] = test;
	++test_size;
}

void print_check_error(
	char const* a, char const* b, char const* op, int line, char const* file)
{
	printf("||-[failed]: %s %s %s (%s:%d)\n", a, op, b, file, line);
}