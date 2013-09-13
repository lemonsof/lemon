/**
* 
* @file     fd
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/11
*/
#ifndef LEMON_KERNEL_IO_FD_HPP
#define LEMON_KERNEL_IO_FD_HPP
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace kernel{

	class lemon_fd : private nocopyable
	{
	public:

		lemon_fd():_refs(1){}

		virtual ~lemon_fd(){}

		static lemon_fd* from(lemon_fd_t id)
		{
			return reinterpret_cast<lemon_fd*>(id);
		}

		operator lemon_fd_t()
		{
			return reinterpret_cast<lemon_fd_t>(this);
		}

		size_t addref(){ return ++ _refs; }

		bool release() { return -- _refs == 0; }

	private:

		size_t											_refs;
	};

}}

#endif	//LEMON_KERNEL_IO_FD_HPP