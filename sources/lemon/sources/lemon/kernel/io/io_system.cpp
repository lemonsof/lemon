#include <lemon/assembly.h>
#include <lemon/kernel/io/fd.hpp>
#include <lemon/kernel/io/io_system.hpp>

namespace lemon{namespace kernel{

	lemon_fd_t lemon_basic_io_system::register_fd(lemon_fd * fd)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if(0 == _fds.count(fd))
		{
			_fds.insert(fd);
		}
		else
		{
			fd->addref();
		}

		return *fd;
	}

	lemon_fd* lemon_basic_io_system::open_fd(lemon_fd_t id)
	{
		lemon_fd * fd = lemon_fd::from(id);

		std::unique_lock<std::mutex> lock(_mutex);

		if(0 == _fds.count(fd))
		{
			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_INVALID_FD);

			throw errorCode;
		}

		fd->addref();

		return fd;
	}

	lemon_fd* lemon_basic_io_system::close_fd(lemon_fd_t id)
	{
		lemon_fd * fd = lemon_fd::from(id);

		std::unique_lock<std::mutex> lock(_mutex);

		if(0 != _fds.count(fd))
		{
			if(fd->release()) return fd;
		}

		return nullptr;
	}

}}