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
		void fit();

		void resize(uint32_t size)
			requires default_constructible<T>;
		void resize(uint32_t size, T const& val)
			requires copy_constructible<T>;

		template <typename... Args>
		T& emplace_back(Args&&... args)
			requires constructible_from<T, Args...>;
		void pop_back();

		template <typename... Args>
		uint32_t emplace(uint32_t pos, Args&&... args)
			requires constructible_from<T, Args...>;

		uint32_t insert(uint32_t pos, T const& val, uint32_t count = 1)
			requires copy_constructible<T> && copy_assignable<T>;
		uint32_t insert(uint32_t pos, T&& val)
			requires move_constructible<T> && move_assignable<T>;
		uint32_t insert(uint32_t pos, std::initializer_list<T> ilist)
			requires copy_constructible<T> && copy_assignable<T>;
		// TODO Insert first last

		uint32_t erase(uint32_t pos, uint32_t count = 1)
			requires move_assignable<T>;

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
	void vector<T>::fit()
	{
		if (cap_ > size_)
			realloc(size_);
	}

	template <vector_type T>
	void vector<T>::resize(uint32_t size)
		requires default_constructible<T>
	{
		if (size > size_)
		{
			if (size > cap_)
			{
				if (!cap_)
					realloc(size);
				else
				{
					uint32_t new_cap = cap_;
					while (new_cap < size)
						new_cap *= 2;

					realloc(new_cap);
				}
			}

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
			{
				if (!cap_)
					realloc(size);
				else
				{
					uint32_t new_cap = cap_;
					while (new_cap < size)
						new_cap *= 2;

					realloc(new_cap);
				}
			}

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
	template <typename... Args>
	T& vector<T>::emplace_back(Args&&... args)
		requires constructible_from<T, Args...>
	{
		if (size_ + 1 > cap_)
		{
			if (!cap_)
				realloc(1);
			else
				realloc(cap_ * 2);
		}

		new (arr_ + size_) T(static_cast<Args&&>(args)...);
		++size_;

		return arr_[size_ - 1];
	}

	template <vector_type T>
	void vector<T>::pop_back()
	{
		arr_[size_ - 1].~T();
		--size_;
	}

	template <vector_type T>
	template <typename... Args>
	uint32_t vector<T>::emplace(uint32_t pos, Args&&... args)
		requires constructible_from<T, Args...>
	{
		// TODO don't use generic realloc, to prevent useless move/copy
		if (size_ + 1 > cap_)
		{
			if (!cap_)
				realloc(1);
			else
				realloc(cap_ * 2);
		}

		if (pos == size_)
		{
			new (arr_ + pos) T(static_cast<Args&&>(args)...);
		}
		else
		{
			if constexpr (requires { requires move_constructible<T>; })
				new (arr_ + size_) T(static_cast<T&&>(arr_[size_ - 1]));
			else
				new (arr_ + size_) T(arr_[size_ - 1]);

			for (uint32_t i {size_ - 1}; i > pos; --i)
			{
				if constexpr (requires { requires move_assignable<T>; })
					arr_[i] = static_cast<T&&>(arr_[i - 1]);
				else
				{
					arr_[i] = arr_[i - 1];
					arr_[i - 1].~T();
				}
			}

			if constexpr (requires { requires move_assignable<T>; })
				arr_[pos].~T();
			new (arr_ + pos) T(static_cast<Args&&>(args)...);
		}
		++size_;
		return pos;
	}

	template <vector_type T>
	uint32_t vector<T>::insert(uint32_t pos, T const& val, uint32_t count)
		requires copy_constructible<T> && copy_assignable<T>
	{
		// TODO don't use generic realloc, to prevent useless move/copy
		if (size_ + count > cap_)
		{
			if (!cap_)
				realloc(count);
			else
			{
				uint32_t new_cap = cap_;
				while (new_cap < size_ + count)
					new_cap *= 2;

				realloc(new_cap);
			}
		}

		if (pos == size_)
		{
			for (uint32_t i {pos}; i < pos + count; ++i)
				new (arr_ + i) T(val);
		}
		else
		{
			for (uint32_t i {size_}; i > size_ - count; --i)
				if constexpr (requires { requires move_constructible<T>; })
					new (arr_ + i - 1 + count) T(static_cast<T&&>(arr_[i - 1]));
				else
					new (arr_ + i - 1 + count) T(arr_[i - 1]);

			for (uint32_t i {size_ - count}; i > pos; --i)
			{
				if constexpr (requires { requires move_assignable<T>; })
					arr_[i - 1 + count] = static_cast<T&&>(arr_[i - 1]);
				else
				{
					arr_[i - 1 + count] = arr_[i - 1];
					arr_[i - 1].~T();
				}
			}

			for (uint32_t i {pos}; i < pos + count; ++i)
				arr_[i] = val;
		}

		size_ += count;

		return pos;
	}

	template <vector_type T>
	uint32_t vector<T>::insert(uint32_t pos, [[maybe_unused]] T&& val)
		requires move_constructible<T> && move_assignable<T>
	{
		// TODO don't use generic realloc, to prevent useless move/copy
		if (size_ + 1 > cap_)
		{
			if (!cap_)
				realloc(1);
			else
				realloc(cap_ * 2);
		}

		if (pos == size_)
		{
			new (arr_ + size_) T(static_cast<T&&>(val));
		}
		else
		{
			if constexpr (requires { requires move_constructible<T>; })
				new (arr_ + size_) T(static_cast<T&&>(arr_[size_ - 1]));
			else
				new (arr_ + size_) T(arr_[size_ - 1]);

			for (uint32_t i {size_ - 1}; i > pos; --i)
				arr_[i] = static_cast<T&&>(arr_[i - 1]);

			arr_[pos] = static_cast<T&&>(val);
		}

		++size_;
		return pos;
	}

	template <vector_type T>
	uint32_t vector<T>::insert(uint32_t                                  pos,
	                           [[maybe_unused]] std::initializer_list<T> ilist)
		requires copy_constructible<T> && copy_assignable<T>
	{
		// TODO don't use generic realloc, to prevent useless move/copy
		if (size_ + ilist.size() > cap_)
		{
			if (!cap_)
				realloc(ilist.size());
			else
			{
				uint32_t new_cap = cap_;
				while (new_cap < size_ + ilist.size())
					new_cap *= 2;

				realloc(new_cap);
			}
		}

		if (pos == size_)
		{
			T const* it = ilist.begin();
			for (uint32_t i {pos}; i < pos + ilist.size(); ++i)
			{
				new (arr_ + i) T(*it);
				++it;
			}
		}
		else
		{
			for (uint32_t i {size_}; i > size_ - ilist.size(); --i)
				if constexpr (requires { requires move_constructible<T>; })
					new (arr_ + i - 1 + ilist.size()) T(static_cast<T&&>(arr_[i - 1]));
				else
					new (arr_ + i - 1 + ilist.size()) T(arr_[i - 1]);

			for (uint32_t i {size_ - static_cast<uint32_t>(ilist.size())}; i > pos; --i)
			{
				if constexpr (requires { requires move_assignable<T>; })
					arr_[i - 1 + ilist.size()] = static_cast<T&&>(arr_[i - 1]);
				else
				{
					arr_[i - 1 + ilist.size()] = arr_[i - 1];
					arr_[i - 1].~T();
				}
			}

			T const* it = ilist.begin();
			for (uint32_t i {pos}; i < pos + ilist.size(); ++i)
			{
				arr_[i] = *it;
				++it;
			}
		}

		size_ += ilist.size();
		return pos;
	}

	template <vector_type T>
	uint32_t vector<T>::erase(uint32_t pos, [[maybe_unused]] uint32_t count)
		requires move_assignable<T>
	{
		for (uint32_t i {pos}; i < size_ - count; ++i)
			arr_[i] = static_cast<T&&>(arr_[i + count]);

		for (uint32_t i {size_ - count}; i < size_; ++i)
			arr_[i].~T();

		size_ -= count;
		return pos;
	}

	template <vector_type T>
	void vector<T>::realloc(uint32_t cap)
	{
		T* new_arr = reinterpret_cast<T*>(alloc(sizeof(T) * cap, alignof(T)));
		if constexpr (requires { requires move_constructible<T>; })
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
} // namespace mc