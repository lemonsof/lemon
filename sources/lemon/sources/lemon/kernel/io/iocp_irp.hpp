/**
* 
* @file     iocp_irp
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/12
*/
#ifndef LEMON_KERNEL_IO_IOCP_IRP_HPP
#define LEMON_KERNEL_IO_IOCP_IRP_HPP
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

#ifdef WIN32

namespace lemon{namespace kernel{

	struct lemon_accept_irp : public OVERLAPPED
	{
		lemon_accept_irp(lemon_t source, lemon_fd_t acceptor, lemon_fd_t peer)
			:Source(source),Acceptor(acceptor),Peer(peer)
		{
			lemon_reset_errorinfo(errorCode);
		}

		lemon_t																Source;

		lemon_fd_t															Acceptor;

		lemon_fd_t															Peer;

		lemon_sockaddr														Addresses[2];

		lemon_errno_info													errorCode;
	};

}}

#endif //WIN32
#endif	//LEMON_KERNEL_IO_IOCP_IRP_HPP