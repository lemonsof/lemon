/**
* 
* @file     handle
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/04
*/
#ifndef HELIXXX_HANDLE_HPP
#define HELIXXX_HANDLE_HPP

#include <algorithm>
#include <helix/abi.h>
#include <helixxx/nocopyable.hpp>

namespace helix{

	template<
		typename Handle,
		void(*CloseF)(helix_state,Handle)
	> 
	class basic_handle_obj : private nocopyable
	{
	public:
		typedef Handle										handle_type;

		basic_handle_obj(helix_state S):_S(S),_handle(HELIX_INVALID_HANDLE(handle_type)){}

		basic_handle_obj(helix_state S,handle_type handle):_S(S),_handle(handle){}

		basic_handle_obj(basic_handle_obj && rhs):_S(rhs._S),_handle(rhs.release()){}

		~basic_handle_obj()
		{
			close();
		}

		bool empty() const
		{
			return HELIX_INVALID_HANDLE(handle_type) == _handle;
		}

		handle_type release()
		{
			return reset(HELIX_INVALID_HANDLE(handle_type));
		}

		handle_type reset(handle_type handle)
		{
			std::swap(handle,_handle);

			return handle;
		}


		void close()
		{
			if(!empty()) CloseF(_S,release());
		}

		handle_type native() const { return _handle; }


	private:

		helix_state													_S;

		handle_type													_handle;
	};
}
#endif	//HELIXXX_HANDLE_HPP
