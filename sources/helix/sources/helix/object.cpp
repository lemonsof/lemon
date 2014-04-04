#include <helix/object.hpp>

namespace helix{ namespace impl{
	void* object::operator new(std::size_t size, helix_alloc_t* alloc)
	{
		if (size == 0)
		{
			throw std::bad_alloc();
		}

		void *block = helix_alloc(alloc, size);

		if (block == nullptr)
		{
			throw std::bad_alloc();
		}

		return block;
	}

	void object::operator delete(void * block, helix_alloc_t * alloc)
	{
		if (block == nullptr)
		{
			return;
		}

		helix_free(alloc, block);
	}

	void* object::operator new[](std::size_t size, helix_alloc_t* alloc)
	{
		if (size == 0)
		{
			throw std::bad_alloc();
		}

		void *block = helix_alloc(alloc, size);

		if (block == nullptr)
		{
			throw std::bad_alloc();
		}

		return block;
	}

	void object::operator delete[](void * block, helix_alloc_t * alloc)
	{
		if (block == nullptr)
		{
			return;
		}

		helix_free(alloc, block);
	}
}}