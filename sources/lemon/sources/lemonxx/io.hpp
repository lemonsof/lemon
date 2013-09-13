/**
* 
* @file     io
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/04
*/
#ifndef LEMONXX_IO_HPP
#define LEMONXX_IO_HPP
#include <lemon/abi.h>
#include <lemonxx/handle.hpp>

namespace lemon{namespace sockets{

	class handle : public basic_handle_obj<lemon_fd_t,&lemon_close_socket>
	{
	public:
		typedef basic_handle_obj<lemon_fd_t,&lemon_close_socket> base_type;

		handle(lemon_state S,int af, int type, int protocol):base_type(S)
		{
			reset(lemon_new_socket(S,af,type,protocol));

			lemon_check_throw(S);
		}


	};

}}
#endif	//LEMONXX_IO_HPP