/**
* 
* @file     extension
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/13
*/
#ifndef LEMONXX_EXTENSION_HPP
#define LEMONXX_EXTENSION_HPP
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{

	template<typename Extension>  class basic_extension : private nocopyable
	{
	public:
		void start(){}

		void stop(){}

	public:

		static lemon_extension_vtable make_extension(Extension * ext)
		{
			lemon_extension_vtable vtable = {typeid(Extension),&basic_extension::__stop_extension,&basic_extension::__close_extension};

			return vtable;
		}

	private:

		static void __start_extension(void * userdata,lemon_state S)
		{
			reinterpret_cast<basic_extension*>(userdata)->_S = S;

			reinterpret_cast<Extension*>(userdata)->start();
		}

		static void __close_extension(void * userdata)
		{
			delete (Extension*)userdata;
		}

		static void __stop_extension(void * userdata)
		{
			reinterpret_cast<Extension*>(userdata)->stop();
		}

	private:

		lemon_state													_S;
	};
}
#endif //LEMONXX_EXTENSION_HPP
