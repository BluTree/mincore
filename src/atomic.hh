#pragma once

#include "concepts.hh"
#include "type_traits.hh"

namespace mc
{
	template <typename T>
	concept atomic_storable =
		(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) &&
		is_trivially_copyable_v<T>;

	enum class mem_order : uint8_t
	{
		relaxed = 0,
		consume = 1,
		acquire = 2,
		release = 3,
		acq_rel = 4,
		seq_cst = 5,
	};

	/// @brief Atomic class, using atomic operations for all accesses to the contained
	/// value.
	/// @tparam T type of atomic variable.
	/// @note The implementation is oriented towards x86_64 architecture, but using the
	/// gcc/clang intrinsic function, it shouldn't make it incompatible with ARM based
	/// architectures.
	/// @details Currently incompatible with MSVC since they don't provide the
	/// "standardized" intrinsic functions. See _Interlocked, __iso_volatile, and
	/// _ReadWriteBarrier intrinsics.
	/// Documentation provided by LLVM itself for the intrinsics is outdated (and
	/// redirects to outdated gcc documentation too). The correct and up-to-date
	/// documentation can be found here :
	/// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
	template <atomic_storable T>
	class atomic
	{
	public:
		atomic()
			requires default_constructible<T>
		= default;
		atomic(T val);
		atomic(atomic const&) = delete;

		atomic& operator=(atomic const&) = delete;

		template <mem_order order = mem_order::seq_cst>
		T load()
			requires(order == mem_order::relaxed || order == mem_order::acquire ||
		             order == mem_order::seq_cst);

		template <mem_order order = mem_order::seq_cst>
		void store(T val)
			requires(order == mem_order::relaxed || order == mem_order::release ||
		             order == mem_order::seq_cst);

		T    exchange(T val);
		bool compare_exchange(T& expected, T desired);

		// TODO allow on pointer types, and respect pointer arithmetics
		template <integral T2>
		T fetch_add(T2 count)
			requires(!same_as<T2, bool>) && ((integral<T> && !same_as<T, bool>) ||
		                                     (pointer<T> && !same_as<T, void*>));
		template <integral T2>
		T fetch_sub(T2 count)
			requires(!same_as<T2, bool>) && ((integral<T> && !same_as<T, bool>) ||
		                                     (pointer<T> && !same_as<T, void*>));
		T fetch_and(T count)
			requires integral<T>;
		T fetch_xor(T count)
			requires integral<T>;
		T fetch_or(T count)
			requires integral<T>;
		T fetch_nand(T count)
			requires integral<T>;

	private:
		// The code below is currently not needed with gcc/clang intrinsics, as they
		// retrieve the pointer and its sizeof automatically. Moreover, keeping the same
		// type allows clang (and probably gcc, to verify) to warns on types that would
		// create inefficient atomics and use locks (because of size, alignment).
		// Normally, the implementation should ensure lock-free atomics, but in case some
		// case were forgotten, this is still useful.
		// Still, this code is kept because it may be useful to implement atomics with
		// MSVC intrinsics.
		/*using storage = conditional_t<
		    sizeof(T) == 1, uint8_t,
		    conditional_t<sizeof(T) == 2, uint16_t,
		                  conditional_t<sizeof(T) == 4, uint32_t, uint64_t>>>;

		static storage as_storage(T val);
		static T       as_value(storage val_bytes);*/

		/// @brief value is always aligned by its size, in case of composite structs
		/// containing e.g. uint8_t a,b. Clang issue warnings because the 2 bytes type
		/// should be aligned on 2 bytes (this is the case on uint16_t), and may possibly
		/// fallback on a lock based implementation.
		alignas(sizeof(T)) T val_;
	};

	static_assert(__atomic_always_lock_free(1, 0),
	              "atomic containing 1 byte storage should be lock-free");
	static_assert(__atomic_always_lock_free(2, 0),
	              "atomic containing 2 bytes storage should be lock-free");
	static_assert(__atomic_always_lock_free(4, 0),
	              "atomic containing 4 bytes storage should be lock-free");
	static_assert(__atomic_always_lock_free(8, 0),
	              "atomic containing 8 bytes storage should be lock-free");
}

namespace mc
{

	template <atomic_storable T>
	atomic<T>::atomic(T val)
	: val_ {val}
	{}

	template <atomic_storable T>
	template <mem_order order>
	T atomic<T>::load()
		requires(order == mem_order::relaxed || order == mem_order::acquire ||
	             order == mem_order::seq_cst)
	{
		T ret;
		__atomic_load(&val_, &ret, static_cast<int32_t>(order));

		return ret;
	}

	template <atomic_storable T>
	template <mem_order order>
	void atomic<T>::store(T val)
		requires(order == mem_order::relaxed || order == mem_order::release ||
	             order == mem_order::seq_cst)
	{
		__atomic_store(&val_, &val, static_cast<int32_t>(order));
	}

	template <atomic_storable T>
	T atomic<T>::exchange(T val)
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		__atomic_exchange(&val_, &val, &ret, static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	template <atomic_storable T>
	bool atomic<T>::compare_exchange(T& expected, T desired)
	{
		// TODO memory order, but (visibly) no impact on x86
		bool ret = __atomic_compare_exchange(&val_, &expected, &desired,
		                                     /*weak*/ false,
		                                     static_cast<int32_t>(mem_order::seq_cst),
		                                     static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	template <atomic_storable T>
	template <integral T2>
	T atomic<T>::fetch_add(T2 count)
		requires(!same_as<T2, bool>) &&
	            ((integral<T> && !same_as<T, bool>) || (pointer<T> && !same_as<T, void*>))
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		if constexpr (is_pointer_v<T>)
		{
			ret = __atomic_fetch_add(&val_, count * sizeof(remove_pointer_t<T>),
			                         static_cast<int32_t>(mem_order::seq_cst));
		}
		else
		{
			ret = __atomic_fetch_add(&val_, count,
			                         static_cast<int32_t>(mem_order::seq_cst));
		}

		return ret;
	}

	template <atomic_storable T>
	template <integral T2>
	T atomic<T>::fetch_sub(T2 count)
		requires(!same_as<T2, bool>) &&
	            ((integral<T> && !same_as<T, bool>) || (pointer<T> && !same_as<T, void*>))
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		if constexpr (is_pointer_v<T>)
		{
			ret = __atomic_fetch_sub(&val_, count * sizeof(remove_pointer_t<T>),
			                         static_cast<int32_t>(mem_order::seq_cst));
		}
		else
		{
			ret = __atomic_fetch_sub(&val_, count,
			                         static_cast<int32_t>(mem_order::seq_cst));
		}

		return ret;
	}

	template <atomic_storable T>
	T atomic<T>::fetch_and(T count)
		requires integral<T>
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		ret = __atomic_fetch_and(&val_, count, static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	template <atomic_storable T>
	T atomic<T>::fetch_xor(T count)
		requires integral<T>
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		ret = __atomic_fetch_xor(&val_, count, static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	template <atomic_storable T>
	T atomic<T>::fetch_or(T count)
		requires integral<T>
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		ret = __atomic_fetch_or(&val_, count, static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	template <atomic_storable T>
	T atomic<T>::fetch_nand(T count)
		requires integral<T>
	{
		T ret;
		// TODO memory order, but (visibly) no impact on x86
		ret = __atomic_fetch_nand(&val_, count, static_cast<int32_t>(mem_order::seq_cst));

		return ret;
	}

	/*template <typename T>
	    requires atomic_storable<T>
	typename atomic<T>::storage atomic<T>::as_storage(T val)
	{
	    if constexpr (is_integral_v<T>)
	        return static_cast<storage>(val);
	    else if constexpr (is_pointer_v<T>)
	        return reinterpret_cast<storage>(val);
	    else
	    {
	        storage ret;
	        memcpy(&ret, &val, sizeof(T));
	        return ret;
	    }
	}

	template <typename T>
	    requires atomic_storable<T>
	T atomic<T>::as_value(storage val_bytes)
	{
	    if constexpr (is_integral_v<T>)
	        return static_cast<T>(val_bytes);
	    else if constexpr (is_pointer_v<T>)
	        return reinterpret_cast<T>(val_bytes);
	    else
	    {
	        T ret;
	        memcpy(&ret, &val_bytes, sizeof(T));
	        return ret;
	    }
	}*/
}