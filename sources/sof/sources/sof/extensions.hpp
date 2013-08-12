/**
* 
* @file     extensions
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/11
*/
#ifndef SOF_EXTENSIONS_HPP
#define SOF_EXTENSIONS_HPP
#include <mutex>
#include <sof/abi.h>
#include <sofxx/nocopyable.hpp>

namespace sof{namespace impl{

	class sof_extension_system : private nocopyable
	{
	public:

		const static size_t max_extensions = 32;

		const static size_t buckets_capacity = max_extensions * 2;

		struct extension{ sof_extension_vtable vtable; void * userdata; };

		sof_extension_system();

		~sof_extension_system();

		void stop();

		void new_extension(sof_state S,const sof_extension_vtable * vtable, void * userdata);

		void * get_extension(sof_state S,sof_extension_t id);

	private:

		size_t hash(sof_extension_t id);

	private:

		std::mutex										_mutex;
				
		extension										_buckets[max_extensions * 2];

		size_t											_counter;
	};

}}
#endif //SOF_EXTENSIONS_HPP