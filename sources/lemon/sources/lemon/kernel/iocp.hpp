/**
* 
* @file     iocp
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/03
*/
#ifndef LEMON_KERNEL_IOCP_HPP
#define LEMON_KERNEL_IOCP_HPP
#ifdef WIN32
#include <lemon/abi.h>
#include <Mswsock.h>
#include <lemonxx/nocopyable.hpp>
namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_io_system;

	class lemon_socket : private nocopyable
	{
	public:

		lemon_socket(lemon_io_system * sysm,int af, int type, int protocol);
		
		~lemon_socket();

		lemon_io_system * io_system() { return _sysm; }

		lemon_system* system();

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
#endif	//LEMON_KERNEL_IOCP_HPP