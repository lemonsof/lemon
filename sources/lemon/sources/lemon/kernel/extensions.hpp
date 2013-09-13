/**
* 
* @file     extensions
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/11
*/
#ifndef LEMON_EXTENSIONS_HPP
#define LEMON_EXTENSIONS_HPP
#include <mutex>
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_extension_system : private nocopyable
	{
	public:

		const static size_t max_extensions = 32;

		const static size_t buckets_capacity = max_extensions * 2;

		struct extension{ lemon_extension_vtable vtable; void * userdata; };

		lemon_extension_system();

		~lemon_extension_system();

		void start(lemon_system * sysm) { _system = sysm; }

		void stop();

		void new_extension(lemon_t source,const lemon_extension_vtable * vtable, void * userdata);

		void * get_extension(lemon_t source,lemon_extension_t id);

	private:

		size_t hash(lemon_extension_t id);

	private:

		lemon_system									*_system;

		std::mutex										_mutex;
				
		extension										_buckets[max_extensions * 2];

		size_t											_counter;
	};

}}
#endif //LEMON_EXTENSIONS_HPP
