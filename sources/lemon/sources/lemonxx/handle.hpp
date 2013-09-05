/**
* 
* @file     handle
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/04
*/
#ifndef LEMONXX_HANDLE_HPP
#define LEMONXX_HANDLE_HPP

#include <algorithm>
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{

	template<
		typename Handle,
		void(*CloseF)(lemon_state,Handle)
	> 
	class basic_handle_obj : private nocopyable
	{
	public:
		typedef Handle										handle_type;

		typedef CloseF										close_f;

		basic_handle_obj(lemon_state S):_handle(LEMON_INVALID_HANDLE(handle_type)){}

		basic_handle_obj(lemon_state S,handle_type handle):_S(S),_handle(handle){}

		basic_handle_obj(basic_handle_obj && rhs):_S(rhs._S),_handle(rhs.release()){}

		~basic_handle_obj()
		{
			close();
		}

		bool empty() const
		{
			return LEMON_INVALID_HANDLE(handle_type) == _handle;
		}

		handle_type release()
		{
			return reset(LEMON_INVALID_HANDLE(handle_type));
		}

		handle_type reset(handle_type handle)
		{
			std::swap(handle,_handle);

			return handle;
		}


		void close()
		{
			if(!empty()) close_f(release());
		}

		handle_type native() const { return _handle; }


	private:

		lemon_state													_S;

		handle_type													_handle;
	};
}
#endif	//LEMONXX_HANDLE_HPP