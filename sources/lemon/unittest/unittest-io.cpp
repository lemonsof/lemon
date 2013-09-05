#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class lemon_io_unittest{};

	LEMON_UNITTEST_CASE(lemon_io_unittest,udp_socket_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		sockets::handle socket(sysm,AF_INET,SOCK_STREAM,IPPROTO_TCP);
	}
}}