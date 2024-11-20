#pragma once

#include <stdint.h>

#include "alloc.hh"
#include "concepts.hh"
#include "initializer_list.hh"

namespace mc
{
	template <typename T>
	concept vector_type =
		destructible<T> && (copy_constructible<T> || move_constructible<T>);

	template <vector_type T>
	class vector
	{
	public:
		vector() = default;
		vector(uint32_t size)
			requires default_constructible<T>;
		vector(std::initializer_list<T> ilist)
			requires copy_constructible<T>;
		vector(vector const& other)
			requires copy_constructible<T>;
		// Doesn't need move_constructible<T> since arr_ is moved, not its content
		vector(vector&& other);
		~vector();

		vector& operator=(vector const& other)
			requires copy_constructible<T>;
		// Doesn't need move_constructible<T> since arr_ is moved, not its content
		vector& operator=(vector&& other);

		T& operator[](uint32_t pos);

		uint32_t size() const;
		uint32_t capacity() const;

	private:
		T*       arr_ {nullptr};
		uint32_t size_ {0};
		uint32_t cap_ {0};
	};
}

namespace mc
{
	template <vector_type T>
	vector<T>::vector(uint32_t size)
		requires default_constructible<T>
	: size_ {size}
	, cap_ {size}
	{
		arr_ = reinterpret_cast<T*>(alloc(sizeof(T) * cap_, alignof(T)));
		for (uint32_t i {0}; i < size_; ++i)
			new (arr_ + i) T;
	}

	template <vector_type T>
	vector<T>::vector(std::initializer_list<T> ilist)
		requires copy_constructible<T>
	: size_ {static_cast<uint32_t>(ilist.size())}
	, cap_ {static_cast<uint32_t>(ilist.size())}
	{
		arr_ = reinterpret_cast<T*>(alloc(sizeof(T) * cap_, alignof(T)));
		for (uint32_t i {0}; i < size_; ++i)
			new (arr_ + i) T(ilist.begin()[i]);
	}

	template <vector_type T>
	vector<T>::vector(vector const& other)
		requires copy_constructible<T>
	: size_ {other.size_}
	, cap_ {other.cap_}
	{
		arr_ = reinterpret_cast<T*>(alloc(sizeof(T) * cap_, alignof(T)));
		for (uint32_t i {0}; i < size_; ++i)
			new (arr_ + i) T(other.arr_[i]);
	}

	template <vector_type T>
	vector<T>::vector(vector&& other)
	: arr_ {other.arr_}
	, size_ {other.size_}
	, cap_ {other.cap_}
	{
		other.arr_ = nullptr;
		other.size_ = 0;
		other.cap_ = 0;
	}

	template <vector_type T>
	vector<T>& vector<T>::operator=(vector const& other)
		requires copy_constructible<T>
	{
		for (uint32_t i {0}; i < size_; ++i)
			arr_[i].~T();

		if (arr_)
			free(arr_, cap_, alignof(T));

		size_ = other.size_;
		cap_ = other.cap_;
		arr_ = reinterpret_cast<T*>(alloc(sizeof(T) * cap_, alignof(T)));
		for (uint32_t i {0}; i < size_; ++i)
			new (arr_ + i) T(other.arr_[i]);
	}

	template <vector_type T>
	vector<T>& vector<T>::operator=(vector&& other)
	{
		for (uint32_t i {0}; i < size_; ++i)
			arr_[i].~T();

		if (arr_)
			free(arr_, cap_, alignof(T));

		arr_ = other.arr_;
		size_ = other.size_;
		cap_ = other.cap_;

		other.arr_ = nullptr;
		other.size_ = 0;
		other.cap_ = 0;
	}

	template <vector_type T>
	vector<T>::~vector()
	{
		for (uint32_t i {0}; i < size_; ++i)
			arr_[i].~T();

		if (arr_)
			free(arr_, cap_, alignof(T));
	}

	template <vector_type T>
	T& vector<T>::operator[](uint32_t pos)
	{
		return arr_[pos];
	}

	template <vector_type T>
	uint32_t vector<T>::size() const
	{
		return size_;
	}

	template <vector_type T>
	uint32_t vector<T>::capacity() const
	{
		return cap_;
	}
}