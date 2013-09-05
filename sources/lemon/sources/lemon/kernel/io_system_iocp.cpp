#include <lemon/kernel/iocp.hpp>
#include <lemon/kernel/io_system_iocp.hpp>

#ifdef WIN32

namespace lemon{namespace kernel{

	lemon_socket_t lemon_io_system::make_socket(lemon_t /*source*/,int af,int type, int protocol)
	{
		return (lemon_socket_t)new lemon_socket(this,af,type,protocol);
	}

	void lemon_io_system::close_socket(lemon_t /*source*/,lemon_socket_t socket)
	{
		delete reinterpret_cast<lemon_socket*>(socket);
	}

}}

#endif //