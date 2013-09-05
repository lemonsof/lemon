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

		operator SOCKET() { return _handle; }

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

	inline void bind(lemon_socket* socket, const sockaddr* name, socklen_t len)
	{
		if(SOCKET_ERROR == ::bind(*socket,name,len)){

			lemon_declare_errinfo(errorCode);

			lemon_win32_errno(errorCode,WSAGetLastError());

			throw errorCode;
		}
	}

	inline void listen(lemon_socket* socket,int backlog)
	{
		if(SOCKET_ERROR == ::listen(*socket,backlog)){

			lemon_declare_errinfo(errorCode);

			lemon_win32_errno(errorCode,WSAGetLastError());

			throw errorCode;
		}
	}

	void accept(lemon_socket * socket,size_t timeout);
}}

#endif //WIN32
#endif	//LEMON_KERNEL_IOCP_HPP