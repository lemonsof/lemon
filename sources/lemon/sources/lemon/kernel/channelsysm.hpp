/**
* 
* @file     channelsysm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/20
*/
#ifndef LEMON_CHANNEL_SYSTEM_HYPP
#define LEMON_CHANNEL_SYSTEM_HYPP
#include <lemon/kernel/channel.hpp>

namespace lemon{namespace impl{

	class lemon_system;

	class lemon_channel_system : private nocopyable
	{
	public:
		typedef std::unordered_map<lemon_channel_t,lemon_channel*>	channels_t;

		lemon_channel_system();

		~lemon_channel_system();

		void start(lemon_system * sysm);

		void stop();

	public:

		lemon_channel_t make_channel(lemon_state S,lemon_msg_close_f f,size_t maxlen, int flags);

		void close_channel(lemon_channel_t channel);

		void* recv(lemon_state S,lemon_channel_t channel,size_t timeout);

		void* recv(lemon_state S,const lemon_channel_t *channelist,size_t len, size_t timeout);

		void send(lemon_state S,lemon_channel_t channel, void * block,size_t timeout);

	private:

		std::mutex													_mutex;

		lemon_channel_t												_seq;

		lemon_system												*_sysm;

		channels_t													_channels;
	};
}}
#endif //LEMON_CHANNEL_SYSTEM_HYPP