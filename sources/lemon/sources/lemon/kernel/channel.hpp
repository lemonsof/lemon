/**
* 
* @file     channel
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/23
*/
#ifndef LEMON_KERNEL_CHANNEL_HPP
#define LEMON_KERNEL_CHANNEL_HPP
#include <list>
#include <mutex>
#include <lemon/abi.h>
#include <unordered_map>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace kernel{

	class lemon_actor;

	class lemon_system;

	class lemon_channel : private nocopyable
	{
	protected:

		lemon_channel(lemon_system* sysm,lemon_msg_f f,void * userdata);

		virtual ~lemon_channel(){}

	public:

		static lemon_event_t close_event()
		{
			static void * unknown;

			return (lemon_event_t)&unknown;
		}

		bool release(bool force = false)
		{
			if(force)
			{
				delete this;

				return true;
			}

			if(-- _counter == 0)
			{
				delete this;

				return true;
			}

			return false;
		}

		void addref()
		{
			++ _counter;
		}

		static lemon_channel* from(lemon_channel_t id)
		{
			return reinterpret_cast<lemon_channel*>(id);
		}

		lemon_channel_t id()
		{
			return (lemon_channel_t)this;
		}

		operator lemon_channel_t()
		{
			return id();
		}

		lemon_system* system() { return _system; }

		void close_msg(void *msg);

		void* clone_msg(void *msg);

		virtual bool send(lemon_actor * actor,void * msg,int flags, size_t timeout) = 0;

		virtual void* recv(lemon_actor * actor, int flags, size_t timeout) = 0;

	private:

		lemon_system											*_system;

		int														_counter;

		lemon_msg_f												_f;

		void													*_userdata;
	};

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	class lemon_push_channel : public lemon_channel
	{
	public:
		struct lemon_msg
		{
			lemon_t												source;

			void												*userdata;
		};

		typedef std::list<lemon_msg>							export_queue;

		typedef std::list<lemon_t>								import_queue;


		lemon_push_channel(lemon_system* sysm,size_t maxlen, lemon_msg_f f,void * userdata);

		~lemon_push_channel();

		bool send(lemon_actor * actor,void * msg,int flags, size_t timeout);

		void* recv(lemon_actor * actor, int flags, size_t timeout);

	private:

		std::mutex												_mutex;

		size_t													_length;

		export_queue											_export;

		import_queue											_import;
	};

	//////////////////////////////////////////////////////////////////////////

	class lemon_publish_channel : public lemon_channel
	{
	public:

		struct lemon_msg
		{
			lemon_t												target;

			void												**userdata;

		};

		typedef std::list<lemon_msg>							import_queue;

		lemon_publish_channel(lemon_system* sysm,size_t maxlen, lemon_msg_f f,void * userdata);

		~lemon_publish_channel();

		bool send(lemon_actor * actor,void * msg,int flags, size_t timeout);

		void* recv(lemon_actor * actor, int flags, size_t timeout);

	private:
		size_t													_maxlen;

		lemon_t													_current;

		std::mutex												_mutex;

		import_queue											_import;
	};

	//////////////////////////////////////////////////////////////////////////

	class lemon_request_channel : public lemon_channel
	{
	public:

		struct lemon_request
		{
			size_t												id;

			lemon_t												source;

			void												*userdata;

		};

		struct lemon_response
		{
			size_t												id;

			lemon_t												source;

			void												*userdata;
		};

		typedef std::unordered_map<lemon_t, lemon_response>		response_map;

		typedef std::list<lemon_request>						import_queue;

		//only one receiver

		lemon_request_channel(lemon_system* sysm,size_t maxlen, lemon_msg_f f,void * userdata);

		~lemon_request_channel();

		bool send(lemon_actor * actor,void * msg,int flags, size_t timeout);

		void* recv(lemon_actor * actor, int flags, size_t timeout);

	private:

		bool send_import(lemon_actor * actor, void * msg, int flags, size_t timeout);

		void* recv_import(lemon_actor * actor, int flags, size_t timeout);

		bool send_export(lemon_actor * actor, void * msg, int flags, size_t timeout);

		void* recv_export(lemon_actor * actor, int flags, size_t timeout);

	private:

		size_t													_id;

		std::mutex												_mutex;

		size_t													_maxlen;

		lemon_t													_responder;

		lemon_request											_currentMsg;

		response_map											_response_map;

		import_queue											_improt;
	};

}}

#endif //LEMON_KERNEL_CHANNEL_HPP