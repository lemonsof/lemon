/**
* 
* @file     iocp_socket
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/11
*/
#ifndef LEMON_KERNEL_IO_IOCP_SOCKET_HPP
#define LEMON_KERNEL_IO_IOCP_SOCKET_HPP
#include <lemon/kernel/io/fd.hpp>

#ifdef WIN32

#include <MSWSock.h>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_io_system;

	class lemon_socket : public lemon_fd
	{
	public:
		lemon_socket(lemon_io_system * sysm,int af, int type, int protocol);

		~lemon_socket();

		lemon_io_system * io_system() { return _sysm; }

		lemon_system* system();

		operator SOCKET() { return _handle; }

		int af() const { return _af;}

		int type() const { return _type;}

		int protocol() const { return _protocol;}

		LPFN_ACCEPTEX acceptex() { return _acceptEx; }

	private:


		lemon_io_system								*_sysm;

		SOCKET										_handle;

		int											_af;

		int											_type;

		int											_protocol;

		LPFN_ACCEPTEX								_acceptEx;

		LPFN_CONNECTEX								_connectEx;

		LPFN_GETACCEPTEXSOCKADDRS					_getAcceptExSockaddrs;
	};
}}

#endif //WIN32


#endif	//LEMON_KERNEL_IO_IOCP_SOCKET_HPP