#pragma once

#include <stdint.h>

using nullptr_t = decltype(nullptr);

namespace std
{
	namespace internal
	{
		struct literal_zero
		{
			consteval literal_zero([[maybe_unused]] nullptr_t zero) {}
		};

		enum value_type : int8_t
		{
			less = -1,
			equivalent = 0,
			equal = 0,
			greater = 1,
			unordered = 2
		};
	}

	class partial_ordering
	{
	public:
		static partial_ordering const less;
		static partial_ordering const equivalent;
		static partial_ordering const greater;
		static partial_ordering const unordered;

		partial_ordering() = default;

		constexpr partial_ordering(internal::value_type v);

		friend constexpr bool operator==(partial_ordering const&,
		                                 partial_ordering const&) = default;
		friend constexpr bool operator==(partial_ordering const& v,
		                                 internal::literal_zero);

		friend constexpr bool operator<(partial_ordering v, internal::literal_zero);
		friend constexpr bool operator<(internal::literal_zero, partial_ordering v);
		friend constexpr bool operator<=(partial_ordering v, internal::literal_zero);
		friend constexpr bool operator<=(internal::literal_zero, partial_ordering v);
		friend constexpr bool operator>(partial_ordering v, internal::literal_zero);
		friend constexpr bool operator>(internal::literal_zero, partial_ordering v);
		friend constexpr bool operator>=(partial_ordering v, internal::literal_zero);
		friend constexpr bool operator>=(internal::literal_zero, partial_ordering v);
		friend constexpr partial_ordering operator<=>(partial_ordering v,
		                                              internal::literal_zero);
		friend constexpr partial_ordering operator<=>(internal::literal_zero,
		                                              partial_ordering v);

	private:
		internal::value_type value_;
	};

	class weak_ordering
	{
	public:
		static weak_ordering const less;
		static weak_ordering const equivalent;
		static weak_ordering const greater;

		weak_ordering() = default;

		constexpr weak_ordering(internal::value_type v);

		constexpr operator partial_ordering() const;

		friend constexpr bool operator==(weak_ordering const&,
		                                 weak_ordering const&) = default;
		friend constexpr bool operator==(weak_ordering const& v, internal::literal_zero);

		friend constexpr bool operator<(weak_ordering v, internal::literal_zero);
		friend constexpr bool operator<(internal::literal_zero, weak_ordering v);
		friend constexpr bool operator<=(weak_ordering v, internal::literal_zero);
		friend constexpr bool operator<=(internal::literal_zero, weak_ordering v);
		friend constexpr bool operator>(weak_ordering v, internal::literal_zero);
		friend constexpr bool operator>(internal::literal_zero, weak_ordering v);
		friend constexpr bool operator>=(weak_ordering v, internal::literal_zero);
		friend constexpr bool operator>=(internal::literal_zero, weak_ordering v);
		friend constexpr weak_ordering operator<=>(weak_ordering v,
		                                           internal::literal_zero);
		friend constexpr weak_ordering operator<=>(internal::literal_zero,
		                                           weak_ordering v);

	private:
		internal::value_type value_;
	};

	class strong_ordering
	{
	public:
		static strong_ordering const less;
		static strong_ordering const equivalent;
		static strong_ordering const equal;
		static strong_ordering const greater;

		strong_ordering() = default;

		constexpr strong_ordering(internal::value_type v);

		constexpr operator partial_ordering() const;
		constexpr operator weak_ordering() const;

		friend constexpr bool operator==(strong_ordering const&,
		                                 strong_ordering const&) = default;
		friend constexpr bool operator==(strong_ordering const& v,
		                                 internal::literal_zero);

		friend constexpr bool operator<(strong_ordering v, internal::literal_zero);
		friend constexpr bool operator<(internal::literal_zero, strong_ordering v);
		friend constexpr bool operator<=(strong_ordering v, internal::literal_zero);
		friend constexpr bool operator<=(internal::literal_zero, strong_ordering v);
		friend constexpr bool operator>(strong_ordering v, internal::literal_zero);
		friend constexpr bool operator>(internal::literal_zero, strong_ordering v);
		friend constexpr bool operator>=(strong_ordering v, internal::literal_zero);
		friend constexpr bool operator>=(internal::literal_zero, strong_ordering v);
		friend constexpr strong_ordering operator<=>(strong_ordering v,
		                                             internal::literal_zero);
		friend constexpr strong_ordering operator<=>(internal::literal_zero,
		                                             strong_ordering v);

	private:
		internal::value_type value_;
	};

}

namespace std
{
	constexpr partial_ordering::partial_ordering(internal::value_type v)
	: value_ {v}
	{}

	constexpr bool operator==(partial_ordering const& v, internal::literal_zero)
	{
		return v.value_ == 0;
	}

	constexpr bool operator<(partial_ordering v, internal::literal_zero)
	{
		return v.value_ == -1;
	}

	constexpr bool operator<(internal::literal_zero, partial_ordering v)
	{
		return v.value_ == 1;
	}

	constexpr bool operator<=(partial_ordering v, internal::literal_zero)
	{
		return v.value_ <= 0;
	}

	constexpr bool operator<=(internal::literal_zero, partial_ordering v)
	{
		return (v.value_ & 1) == v.value_;
	}

	constexpr bool operator>(partial_ordering v, internal::literal_zero)
	{
		return v.value_ == 1;
	}

	constexpr bool operator>(internal::literal_zero, partial_ordering v)
	{
		return v.value_ == -1;
	}

	constexpr bool operator>=(partial_ordering v, internal::literal_zero)
	{
		return (v.value_ & 1) == v.value_;
	}

	constexpr bool operator>=(internal::literal_zero, partial_ordering v)
	{
		return v.value_ <= 0;
	}

	constexpr partial_ordering operator<=>(partial_ordering v, internal::literal_zero)
	{
		return v.value_;
	}

	constexpr partial_ordering operator<=>(internal::literal_zero, partial_ordering v)
	{
		if (v.value_ == -2)
			return v.value_;

		return static_cast<internal::value_type>(-v.value_);
	}

	constexpr partial_ordering const partial_ordering::less(internal::value_type::less);
	constexpr partial_ordering const
		partial_ordering::equivalent(internal::value_type::equivalent);
	constexpr partial_ordering const
		partial_ordering::greater(internal::value_type::greater);
	constexpr partial_ordering const
		partial_ordering::unordered(internal::value_type::unordered);

	constexpr weak_ordering::weak_ordering(internal::value_type v)
	: value_ {v}
	{}

	constexpr weak_ordering::operator partial_ordering() const
	{
		return value_;
	}

	constexpr bool operator==(weak_ordering const& v, internal::literal_zero)
	{
		return v.value_ == 0;
	}

	constexpr bool operator<(weak_ordering v, internal::literal_zero)
	{
		return v.value_ == -1;
	}

	constexpr bool operator<(internal::literal_zero, weak_ordering v)
	{
		return v.value_ == 1;
	}

	constexpr bool operator<=(weak_ordering v, internal::literal_zero)
	{
		return v.value_ <= 0;
	}

	constexpr bool operator<=(internal::literal_zero, weak_ordering v)
	{
		return v.value_ <= 0;
	}

	constexpr bool operator>(weak_ordering v, internal::literal_zero)
	{
		return v.value_ == 1;
	}

	constexpr bool operator>(internal::literal_zero, weak_ordering v)
	{
		return v.value_ == -1;
	}

	constexpr bool operator>=(weak_ordering v, internal::literal_zero)
	{
		return v.value_ >= 0;
	}

	constexpr bool operator>=(internal::literal_zero, weak_ordering v)
	{
		return v.value_ <= 0;
	}

	constexpr weak_ordering operator<=>(weak_ordering v, internal::literal_zero)
	{
		return v.value_;
	}

	constexpr weak_ordering operator<=>(internal::literal_zero, weak_ordering v)
	{
		return static_cast<internal::value_type>(-v.value_);
	}

	constexpr weak_ordering const weak_ordering::less(internal::value_type::less);
	constexpr weak_ordering const
		weak_ordering::equivalent(internal::value_type::equivalent);
	constexpr weak_ordering const weak_ordering::greater(internal::value_type::greater);

	constexpr strong_ordering::strong_ordering(internal::value_type v)
	: value_ {v}
	{}

	constexpr strong_ordering::operator partial_ordering() const
	{
		return value_;
	}

	constexpr strong_ordering::operator weak_ordering() const
	{
		return value_;
	}

	constexpr bool operator==(strong_ordering const& v, internal::literal_zero)
	{
		return v.value_ == 0;
	}

	constexpr bool operator<(strong_ordering v, internal::literal_zero)
	{
		return v.value_ == -1;
	}

	constexpr bool operator<(internal::literal_zero, strong_ordering v)
	{
		return v.value_ == 1;
	}

	constexpr bool operator<=(strong_ordering v, internal::literal_zero)
	{
		return v.value_ <= 0;
	}

	constexpr bool operator<=(internal::literal_zero, strong_ordering v)
	{
		return v.value_ <= 0;
	}

	constexpr bool operator>(strong_ordering v, internal::literal_zero)
	{
		return v.value_ == 1;
	}

	constexpr bool operator>(internal::literal_zero, strong_ordering v)
	{
		return v.value_ == -1;
	}

	constexpr bool operator>=(strong_ordering v, internal::literal_zero)
	{
		return v.value_ >= 0;
	}

	constexpr bool operator>=(internal::literal_zero, strong_ordering v)
	{
		return v.value_ <= 0;
	}

	constexpr strong_ordering operator<=>(strong_ordering v, internal::literal_zero)
	{
		return v.value_;
	}

	constexpr strong_ordering operator<=>(internal::literal_zero, strong_ordering v)
	{
		return static_cast<internal::value_type>(-v.value_);
	}

	constexpr strong_ordering const strong_ordering::less(internal::value_type::less);
	constexpr strong_ordering const
		strong_ordering::equivalent(internal::value_type::equivalent);
	constexpr strong_ordering const strong_ordering::equal(internal::value_type::equal);
	constexpr strong_ordering const
		strong_ordering::greater(internal::value_type::greater);
}
