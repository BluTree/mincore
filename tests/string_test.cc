#include "unit.hh"

#include <string.hh>

#include <string.h>

// TODO not every code paths have been tested. Most are duplicated with the buffer access
// differing
// TODO Add checks for small string capacity (= 16 with default config)
GROUP(string)
{
	TEST(ctor)
	{
		mc::string str0;
		CHECK_EQ(str0.size(), 0)

		mc::string str1(10, 'a');
		CHECK_EQ(str1.size(), 10)
		CHECK_EQ(strcmp(str1.data(), "aaaaaaaaaa"), 0)

		mc::string str2("Hello World!");
		CHECK_EQ(str2.size(), 12)
		CHECK_EQ(strcmp(str2.data(), "Hello World!"), 0)

		mc::string str3(str2, 0, 5);
		CHECK_EQ(str3.size(), 5)
		CHECK_EQ(strcmp(str3.data(), "Hello"), 0)

		mc::string str4("Hello World!", 5);
		CHECK_EQ(str4.size(), 5)
		CHECK_EQ(strcmp(str4.data(), "Hello"), 0)

		mc::string str5(str4);
		CHECK_EQ(str5.size(), 5)
		CHECK_EQ(strcmp(str5.data(), "Hello"), 0)

		mc::string str6(static_cast<mc::string&&>(str5));
		CHECK_EQ(str6.size(), 5)
		CHECK_EQ(strcmp(str6.data(), "Hello"), 0)
		CHECK_EQ(str5.size(), 0); // NOLINT(clang-analyzer-cplusplus.Move

		mc::string str7 {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
		CHECK_EQ(str7.size(), 12)
		CHECK_EQ(strcmp(str7.data(), "Hello World!"), 0)

		mc::string str8(37, 'a');
		CHECK_EQ(str8.size(), 37)
		CHECK_EQ(str8.capacity(), 37)
		for (uint32_t i {0}; i < str8.size(); ++i)
			CHECK_EQ(str8[i], 'a')

		char const* long_str =
			"This is a very long string containing the sequence Hello World!";
		mc::string str9(long_str);
		CHECK_EQ(str9.size(), 63)
		CHECK_EQ(str9.capacity(), 63)
		CHECK_EQ(strcmp(str9.data(), long_str), 0)

		mc::string str10(str9, 25, 25);
		CHECK_EQ(str10.size(), 25)
		CHECK_EQ(str10.capacity(), 25)
		CHECK_EQ(strncmp(str10.data(), long_str + 25, 25), 0)

		mc::string str11(long_str, 25);
		CHECK_EQ(str11.size(), 25)
		CHECK_EQ(str11.capacity(), 25)
		CHECK_EQ(strncmp(str11.data(), long_str, 25), 0)

		mc::string str12(static_cast<mc::string&&>(str11));
		CHECK_EQ(str12.size(), 25)
		CHECK_EQ(str12.capacity(), 25)
		CHECK_EQ(strncmp(str12.data(), long_str, 25), 0)
		CHECK_EQ(str11.size(), 0); // NOLINT(clang-analyzer-cplusplus.Move

		mc::string str13 {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'v', 'e', 'r',
		                  'y', ' ', 'l', 'o', 'n', 'g', ' ', 's', 't', 'r', 'i', 'n', 'g',
		                  ' ', 'c', 'o', 'n', 't', 'a', 'i', 'n', 'i', 'n', 'g', ' ', 't',
		                  'h', 'e', ' ', 's', 'e', 'q', 'u', 'e', 'n', 'c', 'e', ' ', 'H',
		                  'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
		CHECK_EQ(str13.size(), 63)
		CHECK_EQ(str13.capacity(), 63)
		CHECK_EQ(strcmp(str13.data(), long_str), 0)
	}

	TEST(reserve)
	{
		mc::string str0;
		CHECK_EQ(str0.size(), 0)
		CHECK_EQ(str0.capacity(), 15)

		str0.reserve(12);
		CHECK_EQ(str0.size(), 0)
		CHECK_EQ(str0.capacity(), 15)

		str0.reserve(18);
		CHECK_EQ(str0.size(), 0)
		CHECK_EQ(str0.capacity(), 18)

		str0.reserve(32);
		CHECK_EQ(str0.size(), 0)
		CHECK_EQ(str0.capacity(), 32)
	}

	TEST(fit)
	{
		mc::string str0("Hello World");
		CHECK_EQ(str0.size(), 11)
		CHECK_EQ(str0.capacity(), 15)

		str0.fit();
		CHECK_EQ(str0.capacity(), 15)

		mc::string str1;
		str1.reserve(18);
		CHECK_EQ(str1.capacity(), 18)

		str1 = "This is a very long string containing the sequence Hello World!";
		CHECK_EQ(str1.size(), 63)
		CHECK_EQ(str1.capacity(), 72)

		str1.fit();
		CHECK_EQ(str1.capacity(), 63)
	}

	TEST(assign)
	{
		char const* long_str =
			"This is a very long string containing the sequence Hello World!";

		mc::string str0;
		CHECK_EQ(str0.size(), 0)

		str0 = "Hello World!";
		CHECK_EQ(str0.size(), 12)
		CHECK_EQ(strcmp(str0.data(), "Hello World!"), 0)

		str0 = mc::string_view("This is a longer Hello World!");
		CHECK_EQ(str0.size(), 29)
		CHECK_EQ(str0.capacity(), 29)
		CHECK_EQ(strcmp(str0.data(), "This is a longer Hello World!"), 0)

		mc::string str1("Large string for test");
		CHECK_EQ(str1.size(), 21)

		str1 = str0;
		CHECK_EQ(str1.size(), 29)
		CHECK_EQ(str1.capacity(), 42)
		CHECK_EQ(strcmp(str1.data(), "This is a longer Hello World!"), 0)

		mc::string str2;
		CHECK_EQ(str2.size(), 0)

		str2 = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
		CHECK_EQ(str2.size(), 12)
		CHECK_EQ(strcmp(str2.data(), "Hello World!"), 0)

		str2 = static_cast<mc::string&&>(str1);
		CHECK_EQ(str2.size(), 29)
		CHECK_EQ(str2.capacity(), 42)
		CHECK_EQ(strcmp(str2.data(), "This is a longer Hello World!"), 0)
		CHECK_EQ(str1.size(), 0); // NOLINT(clang-analyzer-cplusplus.Move

		mc::string str3;
		CHECK_EQ(str3.size(), 0)

		str3.assign(10, 'a');
		CHECK_EQ(str3.size(), 10)
		CHECK_EQ(strcmp(str3.data(), "aaaaaaaaaa"), 0)

		str3.assign(long_str, 25);
		CHECK_EQ(str3.size(), 25)
		CHECK_EQ(str3.capacity(), 25)
		CHECK_EQ(strncmp(str3.data(), long_str, 25), 0)

		mc::string str4("Hello World!");
		str3.assign(str4);
		CHECK_EQ(str3.size(), 12)
		CHECK_EQ(str3.capacity(), 25)
		CHECK_EQ(strcmp(str3.data(), "Hello World!"), 0)

		mc::string str5;
		CHECK_EQ(str5.size(), 0)
		str5.assign(static_cast<mc::string&&>(str3));
		CHECK_EQ(str5.size(), 12)
		CHECK_EQ(str5.capacity(), 25)
		CHECK_EQ(strcmp(str5.data(), "Hello World!"), 0)
		CHECK_EQ(str3.size(), 0)

		mc::string str6;
		CHECK_EQ(str6.size(), 0)
		str6.assign(mc::string_view("Hello World"), 2, 8);
		CHECK_EQ(str6.size(), 8)
		CHECK_EQ(strcmp(str6.data(), "llo Worl"), 0)
	}

	TEST(insert)
	{
		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.insert(5, 5, 'b');
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 5 || i >= 10)
					CHECK_EQ(str0[i], 'a')
				else
					CHECK_EQ(str0[i], 'b')

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.insert(5, 10, 'b');
			CHECK_EQ(str1.size(), 25)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str1[i], 'a')
				else
					CHECK_EQ(str1[i], 'b')

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(30);
			str2.insert(5, 10, 'b');
			CHECK_EQ(str2.size(), 30)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str2[i], 'a')
				else
					CHECK_EQ(str2[i], 'b')

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.insert(5, 10, 'b');
			CHECK_EQ(str3.size(), 30)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str3[i], 'a')
				else
					CHECK_EQ(str3[i], 'b')
		}

		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.insert(5, "Hello World!", 5);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 5 || i >= 10)
					CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strncmp(str0.data() + 5, "Hello", 5), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.insert(5, "Hello World!");
			CHECK_EQ(str1.size(), 27)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 5 || i >= 17)
					CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strncmp(str1.data() + 5, "Hello World!", 12), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.insert(5, "Hello World!");
			CHECK_EQ(str2.size(), 32)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 5 || i >= 17)
					CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strncmp(str2.data() + 5, "Hello World!", 12), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.insert(5, "Hello World!");
			CHECK_EQ(str3.size(), 32)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 5 || i >= 17)
					CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strncmp(str3.data() + 5, "Hello World!", 12), 0)
		}

		{
			mc::string str0(9, 'a');
			CHECK_EQ(str0.size(), 9)
			str0.insert(5, mc::string("Hello World!"), 6);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strncmp(str0.data() + 5, "World!", 6), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.insert(5, mc::string("Hello World!"), 6);
			CHECK_EQ(str1.size(), 21)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strncmp(str1.data() + 5, "World!", 6), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.insert(5, mc::string("Hello World!"), 6);
			CHECK_EQ(str2.size(), 26)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strncmp(str2.data() + 5, "World!", 6), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.insert(5, mc::string("Hello World!"), 6);
			CHECK_EQ(str3.size(), 26)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strncmp(str3.data() + 5, "World!", 6), 0)
		}

		{
			mc::string str0(9, 'a');
			CHECK_EQ(str0.size(), 9)
			str0.insert(5, mc::string_view("Hello World!"), 6);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strncmp(str0.data() + 5, "World!", 6), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.insert(5, mc::string_view("Hello World!"), 6);
			CHECK_EQ(str1.size(), 21)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strncmp(str1.data() + 5, "World!", 6), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.insert(5, mc::string_view("Hello World!"), 6);
			CHECK_EQ(str2.size(), 26)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strncmp(str2.data() + 5, "World!", 6), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.insert(5, mc::string_view("Hello World!"), 6);
			CHECK_EQ(str3.size(), 26)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 5 || i >= 11)
					CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strncmp(str3.data() + 5, "World!", 6), 0)
		}

		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.insert(5, {'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 5 || i >= 10)
					CHECK_EQ(str0[i], 'a')
				else
					CHECK_EQ(str0[i], 'b')

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.insert(5, {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str1.size(), 25)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str1[i], 'a')
				else
					CHECK_EQ(str1[i], 'b')

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(30);
			str2.insert(5, {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str2.size(), 30)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str2[i], 'a')
				else
					CHECK_EQ(str2[i], 'b')

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.insert(5, {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str3.size(), 30)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 5 || i >= 15)
					CHECK_EQ(str3[i], 'a')
				else
					CHECK_EQ(str3[i], 'b')
		}
	}

	TEST(append)
	{
		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.append(5, 'b');
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 10)
					CHECK_EQ(str0[i], 'a')
				else
					CHECK_EQ(str0[i], 'b')

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.append(10, 'b');
			CHECK_EQ(str1.size(), 25)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 15)
					CHECK_EQ(str1[i], 'a')
				else
					CHECK_EQ(str1[i], 'b')

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(30);
			str2.append(10, 'b');
			CHECK_EQ(str2.size(), 30)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 20)
					CHECK_EQ(str2[i], 'a')
				else
					CHECK_EQ(str2[i], 'b')

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.append(10, 'b');
			CHECK_EQ(str3.size(), 30)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 20)
					CHECK_EQ(str3[i], 'a')
				else
					CHECK_EQ(str3[i], 'b')
		}

		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.append("Hello World!", 5);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < 10; ++i)
				CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strcmp(str0.data() + 10, "Hello"), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.append("Hello World!");
			CHECK_EQ(str1.size(), 27)
			for (uint32_t i = 0; i < 15; ++i)
				CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strcmp(str1.data() + 15, "Hello World!"), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.append("Hello World!");
			CHECK_EQ(str2.size(), 32)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strcmp(str2.data() + 20, "Hello World!"), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.append("Hello World!");
			CHECK_EQ(str3.size(), 32)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strcmp(str3.data() + 20, "Hello World!"), 0)
		}

		{
			mc::string str0(9, 'a');
			CHECK_EQ(str0.size(), 9)
			str0.append(mc::string("Hello World!"), 6);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < 9; ++i)
				CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strcmp(str0.data() + 9, "World!"), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.append(mc::string("Hello World!"), 6);
			CHECK_EQ(str1.size(), 21)
			for (uint32_t i = 0; i < 15; ++i)
				CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strcmp(str1.data() + 15, "World!"), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.append(mc::string("Hello World!"), 6);
			CHECK_EQ(str2.size(), 26)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strcmp(str2.data() + 20, "World!"), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.append(mc::string("Hello World!"), 6);
			CHECK_EQ(str3.size(), 26)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strcmp(str3.data() + 20, "World!"), 0)
		}

		{
			mc::string str0(9, 'a');
			CHECK_EQ(str0.size(), 9)
			str0.append(mc::string_view("Hello World!"), 6);
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < 9; ++i)
				CHECK_EQ(str0[i], 'a')

			CHECK_EQ(strcmp(str0.data() + 9, "World!"), 0)

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.append(mc::string_view("Hello World!"), 6);
			CHECK_EQ(str1.size(), 21)
			for (uint32_t i = 0; i < 15; ++i)
				CHECK_EQ(str1[i], 'a')

			CHECK_EQ(strcmp(str1.data() + 15, "World!"), 0)

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(32);
			str2.append(mc::string_view("Hello World!"), 6);
			CHECK_EQ(str2.size(), 26)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str2[i], 'a')

			CHECK_EQ(strcmp(str2.data() + 20, "World!"), 0)

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.append(mc::string_view("Hello World!"), 6);
			CHECK_EQ(str3.size(), 26)
			for (uint32_t i = 0; i < 20; ++i)
				CHECK_EQ(str3[i], 'a')

			CHECK_EQ(strcmp(str3.data() + 20, "World!"), 0)
		}

		{
			mc::string str0(10, 'a');
			CHECK_EQ(str0.size(), 10)
			str0.append({'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str0.size(), 15)
			for (uint32_t i = 0; i < str0.size(); ++i)
				if (i < 10)
					CHECK_EQ(str0[i], 'a')
				else
					CHECK_EQ(str0[i], 'b')

			mc::string str1(15, 'a');
			CHECK_EQ(str1.size(), 15)
			str1.append({'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str1.size(), 25)
			for (uint32_t i = 0; i < str1.size(); ++i)
				if (i < 15)
					CHECK_EQ(str1[i], 'a')
				else
					CHECK_EQ(str1[i], 'b')

			mc::string str2(20, 'a');
			CHECK_EQ(str2.size(), 20)
			str2.reserve(30);
			str2.append({'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str2.size(), 30)
			for (uint32_t i = 0; i < str2.size(); ++i)
				if (i < 20)
					CHECK_EQ(str2[i], 'a')
				else
					CHECK_EQ(str2[i], 'b')

			mc::string str3(20, 'a');
			CHECK_EQ(str3.size(), 20)
			str3.append({'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'});
			CHECK_EQ(str3.size(), 30)
			for (uint32_t i = 0; i < str3.size(); ++i)
				if (i < 20)
					CHECK_EQ(str3[i], 'a')
				else
					CHECK_EQ(str3[i], 'b')
		}
	}
}
