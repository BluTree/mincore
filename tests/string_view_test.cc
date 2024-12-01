#include "unit.hh"

#include <string_view.hh>

#include <string.h>

GROUP(string_view)
{
	TEST(ctor)
	{
		mc::string_view sv0;
		CHECK_EQ(sv0.size(), 0);
		CHECK_EQ(sv0.data(), nullptr);
		CHECK_EQ(sv0.empty(), true);

		char const* lit = "Hello World!";

		mc::string_view sv1(lit);
		CHECK_EQ(sv1.size(), strlen(lit));
		CHECK_EQ(sv1.data(), lit);

		mc::string_view sv2(lit, 5);
		CHECK_EQ(sv2.size(), 5);
		CHECK_EQ(sv2.data(), lit);
	}

	TEST(remove_prefix)
	{
		mc::string_view sv0("Hello World!");

		sv0.remove_prefix(6);
		CHECK_EQ(sv0.size(), 6);
		CHECK_EQ(strncmp(sv0.data(), "World!", sv0.size()), 0);
	}

	TEST(remove_suffix)
	{
		mc::string_view sv0("Hello World!");

		sv0.remove_suffix(6);
		CHECK_EQ(sv0.size(), 6);
		CHECK_EQ(strncmp(sv0.data(), "Hello ", sv0.size()), 0);
	}

	TEST(substr)
	{
		mc::string_view sv0("Hello World!");

		mc::string_view sv1 = sv0.substr(0, 6);
		CHECK_EQ(sv1.size(), 6);
		CHECK_EQ(strncmp(sv1.data(), "Hello ", sv1.size()), 0);

		mc::string_view sv2 = sv0.substr(6);
		CHECK_EQ(sv2.size(), 6);
		CHECK_EQ(strncmp(sv2.data(), "World!", sv2.size()), 0);

		mc::string_view sv3 = sv0.substr(3, 6);
		CHECK_EQ(sv3.size(), 6);
		CHECK_EQ(strncmp(sv3.data(), "lo Wor", sv3.size()), 0);
	}

	TEST(compare)
	{
		mc::string_view sv0;

		int32_t res0 = sv0.compare(sv0);
		CHECK_EQ(res0, 0);

		int32_t res1 = sv0.compare("Hello World!");
		CHECK_LT(res1, 0);

		mc::string_view sv1("Hello World!");

		int32_t res2 = sv1.compare(sv0);
		CHECK_GT(res2, 0);

		int32_t res3 = sv1.compare("Hello World!");
		CHECK_EQ(res3, 0);

		int32_t res4 = sv1.compare("Hello World!!");
		CHECK_LT(res4, 0);
	}

	TEST(starts_with)
	{
		mc::string_view sv0("Hello World!");

		bool res0 = sv0.starts_with("Hello");
		CHECK_EQ(res0, true);

		bool res1 = sv0.starts_with("World!");
		CHECK_EQ(res1, false);

		bool res2 = sv0.starts_with('H');
		CHECK_EQ(res2, true);

		bool res3 = sv0.starts_with('W');
		CHECK_EQ(res3, false);
	}

	TEST(ends_with)
	{
		mc::string_view sv0("Hello World!");

		bool res0 = sv0.ends_with("Hello");
		CHECK_EQ(res0, false);

		bool res1 = sv0.ends_with("World!");
		CHECK_EQ(res1, true);

		bool res2 = sv0.ends_with('o');
		CHECK_EQ(res2, false);

		bool res3 = sv0.ends_with('!');
		CHECK_EQ(res3, true);
	}

	TEST(contains)
	{
		mc::string_view sv0("Hello World!");

		bool res0 = sv0.contains("Hello");
		CHECK_EQ(res0, true);

		bool res1 = sv0.contains("World!");
		CHECK_EQ(res1, true);

		bool res2 = sv0.contains("World !");
		CHECK_EQ(res2, false);

		bool res3 = sv0.contains('H');
		CHECK_EQ(res3, true);

		bool res4 = sv0.contains('W');
		CHECK_EQ(res4, true);

		bool res5 = sv0.contains('Z');
		CHECK_EQ(res5, false);
	}

	TEST(find)
	{
		mc::string_view sv0("Hello World!");

		uint32_t res0 = sv0.find("Hello");
		CHECK_EQ(res0, 0);

		uint32_t res1 = sv0.find("World!");
		CHECK_EQ(res1, 6);

		uint32_t res2 = sv0.find("World !");
		CHECK_EQ(res2, UINT32_MAX);

		uint32_t res3 = sv0.find('H');
		CHECK_EQ(res3, 0);

		uint32_t res4 = sv0.find('W');
		CHECK_EQ(res4, 6);

		uint32_t res5 = sv0.find('Z');
		CHECK_EQ(res5, UINT32_MAX);

		mc::string_view sv1("This is a very long string containing 'Hello World!', so "
		                    "you should use 'pos' argument to optimize the find call");

		uint32_t res6 = sv1.find("Hello", 25);
		CHECK_EQ(res6, 39);

		uint32_t res7 = sv1.find("Hello", 45);
		CHECK_EQ(res7, UINT32_MAX);

		uint32_t res8 = sv1.find("World!", 35);
		CHECK_EQ(res8, 45);

		uint32_t res9 = sv1.find("World !", 28);
		CHECK_EQ(res9, UINT32_MAX);

		uint32_t res10 = sv1.find('H', 25);
		CHECK_EQ(res10, 39);

		uint32_t res11 = sv1.find('H', 45);
		CHECK_EQ(res11, UINT32_MAX);

		uint32_t res12 = sv1.find('W', 35);
		CHECK_EQ(res12, 45);

		uint32_t res13 = sv1.find('Z');
		CHECK_EQ(res13, UINT32_MAX);
	}

	TEST(rfind)
	{
		mc::string_view sv0("Hello World!");

		uint32_t res0 = sv0.rfind("Hello");
		CHECK_EQ(res0, 0);

		uint32_t res1 = sv0.rfind("World!");
		CHECK_EQ(res1, 6);

		uint32_t res2 = sv0.rfind("World !");
		CHECK_EQ(res2, UINT32_MAX);

		uint32_t res3 = sv0.rfind('H');
		CHECK_EQ(res3, 0);

		uint32_t res4 = sv0.rfind('W');
		CHECK_EQ(res4, 6);

		uint32_t res5 = sv0.rfind('Z');
		CHECK_EQ(res5, UINT32_MAX);

		mc::string_view sv1("This is a very long string containing 'Hello World!', so "
		                    "you should use 'pos' argument to optimize the rfind call");

		uint32_t res6 = sv1.rfind("Hello", 45);
		CHECK_EQ(res6, 39);

		uint32_t res7 = sv1.rfind("Hello", 25);
		CHECK_EQ(res7, UINT32_MAX);

		uint32_t res8 = sv1.rfind("World!", 55);
		CHECK_EQ(res8, 45);

		uint32_t res9 = sv1.rfind("World !", 28);
		CHECK_EQ(res9, UINT32_MAX);

		uint32_t res10 = sv1.rfind('H', 45);
		CHECK_EQ(res10, 39);

		uint32_t res11 = sv1.rfind('H', 25);
		CHECK_EQ(res11, UINT32_MAX);

		uint32_t res12 = sv1.rfind('W', 55);
		CHECK_EQ(res12, 45);

		uint32_t res13 = sv1.rfind('Z');
		CHECK_EQ(res13, UINT32_MAX);
	}
}