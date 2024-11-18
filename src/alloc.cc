#include "alloc.hh"

#include <stddef.h>
#include <stdlib.h>

namespace mc
{
	namespace
	{
		void* default_alloc(uint64_t size, uint32_t alignment, [[maybe_unused]] void* ud)
		{
			if (alignment <= alignof(max_align_t))
				return malloc(size);
			else
#ifdef _WIN32
				return _aligned_malloc(size, alignment);
#else
				return aligned_alloc(size, alignment);
#endif
		}

		void default_free(void*                     ptr,
		                  [[maybe_unused]] uint64_t size,
		                  uint32_t                  alignment,
		                  [[maybe_unused]] void*    ud)
		{
			if (alignment <= alignof(max_align_t))
				::free(ptr);
			else
#ifdef _WIN32
				_aligned_free(ptr);
#else
				::free(ptr);
#endif
		}
	}

	alloc_funcs g_alloc {default_alloc, default_free, nullptr};

	void set_alloc_funcs(alloc_func* alloc, free_func* free, void* ud)
	{
		g_alloc.alloc = alloc;
		g_alloc.free = free;
		g_alloc.ud = ud;
	}

	void* alloc(uint64_t size, uint32_t alignment)
	{
		return g_alloc.alloc(size, alignment, g_alloc.ud);
	}

	void free(void* ptr, uint64_t size, uint32_t alignment)
	{
		g_alloc.free(ptr, size, alignment, g_alloc.ud);
	}
}