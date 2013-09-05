/**
* 
* @file     io_system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/03
*/
#ifndef LEMON_IO_SYSTEM_HPP
#define LEMON_IO_SYSTEM_HPP
#include <lemon/abi.h>
#include <lemon/kernel/socket_iocp.hpp>

#ifdef WIN32

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_io_system : private nocopyable
	{
	public:

		lemon_io_system(){}

		~lemon_io_system(){}

		void start(lemon_system * sysm){ _sysm = sysm; }

		void stop(){}

		lemon_system* system() { return _sysm; }

	public:

		lemon_socket_t make_socket(lemon_t source,int af,int type, int protocol);

		void close_socket(lemon_t source,lemon_socket_t socket);

	private:

		lemon_system								*_sysm;
	};

}}

#endif //WIN32

#endif	//LEMON_IO_SYSTEM_HPP