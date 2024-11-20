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
		vector(uint32_t size, T const& val)
			requires copy_constructible<T>;
		// TODO ctor first last
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

		bool     empty() const;
		uint32_t size() const;
		uint32_t capacity() const;

		T&       operator[](uint32_t pos);
		T const& operator[](uint32_t pos) const;
		T&       front();
		T const& front() const;
		T&       back();
		T const& back() const;

		void clear();
		void reserve(uint32_t cap_);
		void resize(uint32_t size)
			requires default_constructible<T>;
		void resize(uint32_t size, T const& val)
			requires copy_constructible<T>;

	private:
		void realloc(uint32_t cap);

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
	vector<T>::vector(uint32_t size, T const& val)
		requires copy_constructible<T>
	: size_ {size}
	, cap_ {size}
	{
		arr_ = reinterpret_cast<T*>(alloc(sizeof(T) * cap_, alignof(T)));
		for (uint32_t i {0}; i < size_; ++i)
			new (arr_ + i) T(val);
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
	bool vector<T>::empty() const
	{
		return size_ == 0;
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

	template <vector_type T>
	T& vector<T>::operator[](uint32_t pos)
	{
		return arr_[pos];
	}

	template <vector_type T>
	T const& vector<T>::operator[](uint32_t pos) const
	{
		return arr_[pos];
	}

	template <vector_type T>
	T& vector<T>::front()
	{
		return arr_;
	}

	template <vector_type T>
	T const& vector<T>::front() const
	{
		return arr_;
	}

	template <vector_type T>
	T& vector<T>::back()
	{
		return arr_ + size_;
	}

	template <vector_type T>
	T const& vector<T>::back() const
	{
		return arr_ + size_;
	}

	template <vector_type T>
	void vector<T>::clear()
	{
		for (uint32_t i {0}; i < size_; ++i)
			arr_[i].~T();

		size_ = 0;
	}

	template <vector_type T>
	void vector<T>::reserve(uint32_t cap)
	{
		if (cap > cap_)
			realloc(cap);
	}

	template <vector_type T>
	void vector<T>::resize(uint32_t size)
		requires default_constructible<T>
	{
		if (size > size_)
		{
			if (size > cap_)
				realloc(cap);

			for (uint32_t i {size_}; i < size; ++i)
				new (arr_ + i) T;
		}
		else
		{
			for (uint32_t i {size_ + 1}; i > size; --i)
				arr_[i - 1].~T();
		}

		size_ = size;
	}

	template <vector_type T>
	void vector<T>::resize(uint32_t size, T const& val)
		requires copy_constructible<T>
	{
		if (size > size_)
		{
			if (size > cap_)
				realloc(cap);

			for (uint32_t i {size_}; i < size; ++i)
				new (arr_ + i) T(val);
		}
		else
		{
			for (uint32_t i {size_ + 1}; i > size; --i)
				arr_[i - 1].~T();
		}

		size_ = size;
	}

	template <vector_type T>
	void vector<T>::realloc(uint32_t cap)
	{
		T* new_arr = alloc(sizeof(T) * cap, alignof(T));
		if constexpr (requires { requires move_constructible<T> })
		{
			for (uint32_t i {0}; i < size_; ++i)
			{
				new (new_arr + i) T(static_cast<T&&>(arr_[i]));
				arr_[i].~T();
			}
		}
		else
		{
			for (uint32_t i {0}; i < size_; ++i)
			{
				new (new_arr + i) T(arr_[i]);
				arr_[i].~T();
			}
		}

		free(arr_, sizeof(T) * cap_, alignof(T));
		cap_ = cap;
		arr_ = new_arr;
	}
}