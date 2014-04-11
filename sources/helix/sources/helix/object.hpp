/**
* 
* @file     object
* @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_OBJECT_HPP
#define HELIX_OBJECT_HPP
#include <utility>
#include <stdexcept>
#include <helix/abi.h>
#include <type_traits>
#include <helix++/nocopyable.hpp>

namespace helix{ namespace impl{

	class object : private nocopyable
	{
	public:
		virtual ~object(){}
                
                static void* operator new(std::size_t )
                {
                    throw std::bad_alloc();
                }
                
                static void operator delete(void *)
                {
                    throw std::bad_alloc();
                }

		static void* operator new(std::size_t size, helix_alloc_t* alloc);

		static void operator delete(void * block, helix_alloc_t * alloc);

		static void* operator new[](std::size_t size, helix_alloc_t* alloc);

		static void operator delete[](void * block,helix_alloc_t * alloc);
	};

	template<typename T>
	static void release(T *t, helix_alloc_t * alloc)
	{
		static_assert(std::is_base_of<object,T>::value,"the T must object's child class");

		t->~T();

		T::operator delete(t, alloc);
	}

	template<typename T>
	static void release(T *t, size_t counter, helix_alloc_t * alloc)
	{
		static_assert(std::is_base_of<object, T>::value, "the T must object's child class");

		for (size_t i = 0; i < counter; ++counter)
		{
			t[i]->~T();
		}

		T::operator delete[](t, alloc);
	}
} }

#endif	//HELIX_OBJECT_HPP