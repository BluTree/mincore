#include "unit.hh"

#include <string.hh>

#include <string.h>

GROUP(string)
{
	TEST(ctor)
	{
		mc::string str0;
		CHECK_EQ(str0.size(), 0);

		mc::string str1(10, 'a');
		CHECK_EQ(str1.size(), 10);
		CHECK_EQ(strcmp(str1.data(), "aaaaaaaaaa"), 0);

		mc::string str2("Hello World!");
		CHECK_EQ(str2.size(), 12);
		CHECK_EQ(strcmp(str2.data(), "Hello World!"), 0);

		mc::string str3(str2, 0, 5);
		CHECK_EQ(str3.size(), 5);
		CHECK_EQ(strcmp(str3.data(), "Hello"), 0);

		mc::string str4("Hello World!", 5);
		CHECK_EQ(str4.size(), 5);
		CHECK_EQ(strcmp(str4.data(), "Hello"), 0);

		mc::string str5(str4);
		CHECK_EQ(str5.size(), 5);
		CHECK_EQ(strcmp(str5.data(), "Hello"), 0);

		mc::string str6(static_cast<mc::string&&>(str5));
		CHECK_EQ(str6.size(), 5);
		CHECK_EQ(strcmp(str6.data(), "Hello"), 0);
		CHECK_EQ(str5.size(), 0);

		mc::string str7 {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
		CHECK_EQ(str7.size(), 12)
		CHECK_EQ(strcmp(str7.data(), "Hello World!"), 0);

		mc::string str8(37, 'a');
		CHECK_EQ(str8.size(), 37);
		CHECK_EQ(str8.capacity(), 37);
		for (uint32_t i {0}; i < str8.size(); ++i)
			CHECK_EQ(str8[i], 'a');

		char const* long_str =
			"This is a very long string containing the sequence Hello World!";
		mc::string str9(long_str);
		CHECK_EQ(str9.size(), 63);
		CHECK_EQ(str9.capacity(), 63);
		CHECK_EQ(strcmp(str9.data(), long_str), 0);

		mc::string str10(str9, 25, 25);
		CHECK_EQ(str10.size(), 25);
		CHECK_EQ(str10.capacity(), 25);
		CHECK_EQ(strncmp(str10.data(), long_str + 25, 25), 0);

		mc::string str11(long_str, 25);
		CHECK_EQ(str11.size(), 25);
		CHECK_EQ(str11.capacity(), 25);
		CHECK_EQ(strncmp(str11.data(), long_str, 25), 0);

		mc::string str12(static_cast<mc::string&&>(str11));
		CHECK_EQ(str12.size(), 25);
		CHECK_EQ(str12.capacity(), 25);
		CHECK_EQ(strncmp(str12.data(), long_str, 25), 0);
		CHECK_EQ(str11.size(), 0);

		mc::string str13 {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'v', 'e', 'r',
		                  'y', ' ', 'l', 'o', 'n', 'g', ' ', 's', 't', 'r', 'i', 'n', 'g',
		                  ' ', 'c', 'o', 'n', 't', 'a', 'i', 'n', 'i', 'n', 'g', ' ', 't',
		                  'h', 'e', ' ', 's', 'e', 'q', 'u', 'e', 'n', 'c', 'e', ' ', 'H',
		                  'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
		CHECK_EQ(str13.size(), 63)
		CHECK_EQ(str13.capacity(), 63)
		CHECK_EQ(strcmp(str13.data(), long_str), 0);
	}
}