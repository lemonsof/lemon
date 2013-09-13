/**
* 
* @file     iocp_io_system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/11
*/
#ifndef LEMON_IO_IOCP_IO_SYSTEM_HPP
#define LEMON_IO_IOCP_IO_SYSTEM_HPP
#include <mutex>
#include <lemon/kernel/io/io_system.hpp>

#ifdef WIN32

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_io_system : public lemon_basic_io_system
	{
	public:

		lemon_io_system(){}

		~lemon_io_system(){}

	public:

		void start(lemon_system * sysm);

		void stop();

	public:

		lemon_fd_t make_socket(lemon_t source,int af, int type, int protocol);

		void close_socket(lemon_t source,lemon_fd_t id);

		lemon_fd_t accept(lemon_t source, lemon_fd_t socket);

	private:

		std::mutex													_acceptorMutex;
	};

}}

#endif //WIN32

#endif	//LEMON_IO_IOCP_IO_SYSTEM_HPP