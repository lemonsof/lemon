/**
* 
* @file     stack
* @brief    Copyright (C) 2014   All Rights Reserved 
* @author   
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_STACK_HPP
#define HELIX_STACK_HPP
#include <utility>
#include <helix/object.hpp>

namespace helix{namespace impl{
	
	class basic_stack : private nocopyable
	{
	public:
		basic_stack() :_stacksize(0),_block(0) {}

		void create(size_t size,helix_alloc_t *alloc)
		{
			if(!empty())
			{
				release(alloc);
			}

			void * block = helix_alloc(alloc, size);

			memset(block, 0, size);

			_block = (helix_byte_t*)block + size;

			_stacksize = size;
		}

		void release(helix_alloc_t *alloc)
		{
			if(empty()) return;

			void * block = (helix_byte_t*)_block - _stacksize;

			helix_free(alloc,block);

			_block = nullptr;
		}

		operator void*(){ return _block; }

		size_t size() const { return _stacksize; }

		bool empty() const { return _block == nullptr; }


	private:
		size_t					_stacksize;

		void					*_block;
	};
}}
#endif	//HELIX_STACK_HPP