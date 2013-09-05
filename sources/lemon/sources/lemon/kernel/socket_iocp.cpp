#include <lemon/kernel/socket_iocp.hpp>

#ifdef WIN32
#include <lemon/kernel/io_system_iocp.hpp>

namespace lemon{namespace kernel{

	const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

	const static GUID GuidConnectionEx = WSAID_CONNECTEX;

	const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

	template<class LPFN>
	LPFN __winsock_extension(SOCKET socket,const GUID *guid)
	{
		DWORD dwBytes;

		LPFN fn;

		if(SOCKET_ERROR == WSAIoctl(socket, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			(LPVOID)guid, 
			sizeof(GUID),
			&fn, 
			sizeof(fn), 
			&dwBytes, 
			NULL, 
			NULL))
		{
			lemon_declare_errinfo(errorCode);

			lemon_win32_errno(errorCode,WSAGetLastError());

			throw errorCode;
		}

		return fn;
	}

	lemon_socket::lemon_socket(lemon_io_system *sysm,int af, int type, int protocol)
		:_sysm(sysm),_af(af),_type(type),_protocol(protocol)
	{
		_handle = WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED);

		if(INVALID_SOCKET == _handle){

			lemon_declare_errinfo(errorCode);

			lemon_win32_errno(errorCode,WSAGetLastError());

			throw errorCode;
		}

		try
		{
			_acceptEx = __winsock_extension<LPFN_ACCEPTEX>(_handle,&GuidAcceptEx);

			_connectEx = __winsock_extension<LPFN_CONNECTEX>(_handle,&GuidConnectionEx);

			_getAcceptExSockaddrs = __winsock_extension<LPFN_GETACCEPTEXSOCKADDRS>(_handle,&GuidGetAcceptExSockaddrs);

		}
		catch(...)
		{
			::closesocket((int)_handle);

			throw;
		}

	}

	lemon_socket::~lemon_socket()
	{
		::closesocket((int)_handle);
	}

	lemon_system* lemon_socket::system()
	{
		return _sysm->system();
	}


	void accept(lemon_socket * socket,size_t timeout)
	{

	}
}}

#endif //WIN32