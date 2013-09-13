/**
* 
* @file     io_system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/09/11
*/
#ifndef LEMON_IO_IO_SYSTEM_HPP
#define LEMON_IO_IO_SYSTEM_HPP
#include <mutex>
#include <assert.h>
#include <lemon/abi.h>
#include <unordered_set>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace kernel{

	class lemon_fd;

	class lemon_socket;

	class lemon_system;

	class lemon_basic_io_system : private nocopyable
	{
	public:

		typedef std::unordered_set<lemon_fd*>		fd_set_t;

		lemon_basic_io_system():_sysm(nullptr){}
		
		~lemon_basic_io_system(){}

		lemon_system * system() { return _sysm; }

		const lemon_system * system() const { return _sysm; }

		lemon_fd_t register_fd(lemon_fd * fd);

		lemon_fd* open_fd(lemon_fd_t fd);

		lemon_fd* close_fd(lemon_fd_t fd);

	protected:

		void set_system(lemon_system * sysm) 
		{
			assert(_sysm == nullptr);

			_sysm = sysm;
		}


	private:

		std::mutex										_mutex;

		lemon_system									*_sysm;

		fd_set_t										_fds;
	};


	class scope_fd : private nocopyable
	{
	public:
		scope_fd(lemon_basic_io_system * sysm,lemon_fd_t fd):_sysm(*sysm),_fd(fd)
		{

		}

		~scope_fd()
		{
			_sysm.close_fd(_fd);
		}


		operator lemon_fd_t () { return _fd; }
	
	private:

		lemon_basic_io_system							&_sysm;

		lemon_fd_t										_fd;
	};
}}

#endif	//LEMON_IO_IO_SYSTEM_HPP