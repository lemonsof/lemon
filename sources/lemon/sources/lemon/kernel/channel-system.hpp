/**
* 
* @file     channel-system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/23
*/
#ifndef LEMON_KERNEL_CHANNEL_SYSTEM_HPP
#define LEMON_KERNEL_CHANNEL_SYSTEM_HPP
#include <mutex>
#include <unordered_set>
#include <lemon/kernel/channel.hpp>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_channel_system : private nocopyable
	{
	public:

		typedef std::unordered_set<lemon_channel_t>			channels_t;

		lemon_channel_system();

		~lemon_channel_system();

		void start(lemon_system * sysm){_system = sysm; }

		void stop();

	public:

		lemon_channel_t make_channel(lemon_t source,int type,size_t maxlen,lemon_msg_close_f f,void * userdata);

		void close_channel(lemon_t source,lemon_channel_t channel);

		bool send(lemon_t source,lemon_channel_t id,void * msg, int flags, size_t timeout);

		void* recv(lemon_t source,lemon_channel_t id, int flags, size_t timeout);

	private:

		std::mutex											_mutex;

		lemon_system										*_system;

		channels_t											_channels;
	};

}}
#endif //LEMON_KERNEL_CHANNEL_SYSTEM_HPP