#include <lemon/kernel/func.hpp>
#include <lemon/kernel/actor.hpp>
#include <lemon/kernel/io/iocp_io_system.hpp>

#ifdef WIN32
#include <lemonxx/mutex.hpp>
#include <lemon/kernel/io/iocp_irp.hpp>
#include <lemon/kernel/io/iocp_socket.hpp>

namespace lemon{namespace kernel{

	typedef basic_lock<std::unique_lock<std::mutex>> lock_wrapper;

	void lemon_io_system::start(lemon_system * sysm)
	{
		set_system(sysm);
	}

	void lemon_io_system::stop()
	{

	}


	lemon_fd_t lemon_io_system::make_socket(lemon_t /*source*/,int af, int type, int protocol)
	{
		return register_fd(new lemon_socket(this,af,type,protocol));
	}

	void lemon_io_system::close_socket(lemon_t /*source*/,lemon_fd_t id)
	{
		auto fd = close_fd(id);

		if(fd) delete fd;
	}

	lemon_fd_t lemon_io_system::accept(lemon_t source, lemon_fd_t id)
	{
		lemon_socket* socket = (lemon_socket*)open_fd(id);

		scope_fd acceptor(this,id);

		lemon_fd_t peer = make_socket(source,socket->af(),socket->type(),socket->protocol());

		auto irp = new lemon_accept_irp(source,acceptor,peer);

		std::unique_lock<std::mutex> lock(_acceptorMutex);

		if(!socket->acceptex()(
			*socket,
			*(lemon_socket*)lemon_socket::from(peer),
			irp->Addresses,
			0,
			sizeof(lemon_sockaddr),
			sizeof(lemon_sockaddr),
			NULL,
			irp))
		{
			if(ERROR_IO_PENDING != WSAGetLastError())
			{
				delete irp;

				lemon_declare_errinfo(errorCode);

				lemon_win32_errno(errorCode,WSAGetLastError());

				throw errorCode;
			}
		}

		lemon_event_t events [] = { (lemon_event_t)irp };

		wait(lemon_actor::from(source), lock_wrapper(lock), cbuff(events),lemon_infinite);

		if(lemon_failed(irp->errorCode))
		{
			close_socket(source,peer);

			lemon_errno_info errorCode = irp->errorCode;

			delete irp;

			throw errorCode;
		}

		delete irp;

		return peer;
	}

}}

#endif //WIN32