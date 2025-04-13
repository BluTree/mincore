#pragma once

namespace mc
{
	namespace __internal
	{
		template <typename T, bool = __is_enum(T)>
		struct _underlying_type
		{
			using type = __underlying_type(T);
		};

		template <typename T>
		struct _underlying_type<T, false>
		{
			using type = void;
		};
	}

	template <typename T>
	using underlying_type = __internal::_underlying_type<T>::type;

	template <typename T>
	underlying_type<T> to_underlying(T e)
	{
		return static_cast<underlying_type<T>>(e);
	}
}