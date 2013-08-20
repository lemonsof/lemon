#include <lemonxx/actor.hpp>
#include <lemonxx/mutex.hpp>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/channel.hpp>

namespace lemon{namespace impl{

	lemon_channel::lemon_channel(lemon_system * sysm,lemon_msg_close_f f, size_t maxlen,int flags)
		:_sysm(sysm)
		,_f(f)
		,_maxlen(maxlen)
		,_flags(flags)
	{
		if(!(_flags & LEMON_CHANNEL_SEND) && !(_flags & LEMON_CHANNEL_RECV))
		{
			_flags |= LEMON_CHANNEL_SEND;

			_flags |= LEMON_CHANNEL_RECV;
		}

		if(_flags & LEMON_RECV_NOBLOCK)
		{
			if(!(_flags & LEMON_CHANNEL_RECV))
			{
				_flags ^= LEMON_RECV_NOBLOCK;
			}
		}

		if(_flags & LEMON_SEND_NOBLOCK)
		{
			if(!(_flags & LEMON_CHANNEL_SEND))
			{
				_flags ^= LEMON_SEND_NOBLOCK;
			}
		}
	}

	lemon_channel::~lemon_channel()
	{
		for(auto msg : _Q)
		{
			if(_f) _f((lemon_state)_sysm->get_main_runq()->main_actor(),msg.msg);
		}

		for(auto receiver : _recvQ)
		{
			lemon_event_t evts = {LEMON_CLOSE_CHANNEL_EVT};

			_sysm->notify(receiver,&evts,1);
		}
	}
}}