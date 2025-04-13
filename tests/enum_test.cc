#include "unit.hh"

#include <stdint.h>

#include <concepts.hh>
#include <enum.hh>

GROUP(enum)
{
	enum class a : uint32_t
	{
		val1,
		val2
	};

	enum class b : int8_t
	{
		val1 = -1,
		val2 = -2
	};

	TEST(underlying_type)
	{
		using a_type = mc::underlying_type<a>;
		using b_type = mc::underlying_type<b>;

		using invalid_type = mc::underlying_type<uint32_t>;

		constexpr bool a_eq = mc::same_as<a_type, uint32_t>;
		constexpr bool b_eq = mc::same_as<b_type, int8_t>;
		constexpr bool invalid_eq = mc::same_as<invalid_type, void>;

		CHECK_EQ(a_eq, true);
		CHECK_EQ(b_eq, true);
		CHECK_EQ(invalid_eq, true);
	}

	TEST(to_underlying)
	{
		CHECK_EQ(mc::to_underlying(a::val1), 0);
		CHECK_EQ(mc::to_underlying(a::val2), 1);

		CHECK_EQ(mc::to_underlying(b::val1), -1);
		CHECK_EQ(mc::to_underlying(b::val2), -2);
	}
}