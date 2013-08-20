#include <assert.h>
#include <lemonxx/mutex.hpp>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/channel.hpp>
#include <lemon/kernel/channelsysm.hpp>

namespace lemon{namespace impl{
	lemon_channel_system::lemon_channel_system():_seq(0),_sysm(nullptr)
	{

	}

	lemon_channel_system::~lemon_channel_system()
	{
		for(auto channel : _channels)
		{
			delete channel.second;
		}
	}

	void lemon_channel_system::start(lemon_system * sysm)
	{
		_sysm = sysm;
	}

	void lemon_channel_system::stop()
	{

	}

	lemon_channel_t lemon_channel_system::make_channel(lemon_state /*S*/,lemon_msg_close_f f,size_t maxlen, int flags)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		for(;;)
		{
			lemon_t id = _seq ++ ;

			if(id != 0 && id != LEMON_INVALID_HANDLE && _channels.find(id) == _channels.end())
			{
				_channels.insert(std::make_pair(id,new lemon_channel(_sysm,f,maxlen,flags)));

				return id;
			}
		}
	}

	void lemon_channel_system::close_channel(lemon_channel_t id)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		auto iter = _channels.find(id);

		if(_channels.end() != iter)
		{
			delete iter->second;

			_channels.erase(iter);
		}
	}

	void* lemon_channel_system::recv(lemon_state S,lemon_channel_t id,size_t timeout)
	{
		typedef basic_lock<std::unique_lock<std::mutex>> lock_t;

		std::unique_lock<std::mutex> lock(_mutex);

		auto iter = _channels.find(id);

		if(_channels.end() == iter)
		{
			throw lemon_raise_errno(S,nullptr,LEMON_CHANNEL_NOT_EXISTS);
		}

		auto channel = iter->second;

		lemon_msg msg = {LEMON_INVALID_HANDLE,0};

		if(!channel->pop(msg))
		{
			if(LEMON_RECV_NOBLOCK & channel->flags()) return nullptr;

			while(!channel->pop(msg))
			{
				channel->wait_recv(S,((lemon_actor*)S)->Id);

				lemon_event_t evt[] = {(lemon_event_t)channel,LEMON_CLOSE_CHANNEL_EVT};

				lemon_log_debug(S,"receiver waiting ...");

				lemon_event_t notifiedEvent = lemon_wait(S,lock_t(lock),evt,sizeof(evt)/sizeof(lemon_event_t),timeout);

				if(LEMON_INVALID_HANDLE == notifiedEvent)
				{
					actor(S).check_throw();

					lemon_log_debug(S,"receiver waiting -- timeout");

					return nullptr;
				}
				else if(LEMON_CLOSE_CHANNEL_EVT == notifiedEvent)
				{
					throw lemon_raise_errno(S,"other actor close the channel",LEMON_CHANNEL_NOT_EXISTS);
				}

				lemon_log_debug(S,"receiver waiting -- success");
			}

			assert(msg.msg);

			assert(msg.sender != LEMON_INVALID_HANDLE);
		}

		if((LEMON_SEND_NOBLOCK & channel->flags()) == 0)
		{
			lemon_event_t evt[] = {(lemon_event_t)channel};

			lemon_log_debug(S,"receiver notify ...");

			lemon_notify(S,msg.sender,evt,sizeof(evt)/sizeof(lemon_event_t));

			lemon_log_debug(S,"receiver notify -- success");
		}

		return msg.msg;
	}

	void* lemon_channel_system::recv(lemon_state S,const lemon_channel_t *channelist,size_t len, size_t timeout)
	{
		typedef basic_lock<std::unique_lock<std::mutex>> lock_t;

		std::vector<lemon_event_t> events;

		std::unique_lock<std::mutex> lock(_mutex);

		lemon_msg msg = {LEMON_INVALID_HANDLE,0};

		for(size_t i = 0; i < len; ++ i)
		{
			auto iter = _channels.find(channelist[i]);

			if(iter  == _channels.end()) continue;

			auto channel = iter->second;

			if(channel->pop(msg))
			{
				if((LEMON_SEND_NOBLOCK & channel->flags()) == 0)
				{
					lemon_event_t evt[] = {(lemon_event_t)channel};

					lemon_log_debug(S,"receiver notify ...");

					lemon_notify(S,msg.sender,evt,sizeof(evt)/sizeof(lemon_event_t));

					lemon_log_debug(S,"receiver notify -- success");
				}

				return msg.msg;
			}

			events.push_back((lemon_event_t)channel);
		}

		if(events.empty())
		{
			throw lemon_raise_errno(S,nullptr,LEMON_CHANNEL_NOT_EXISTS);
		}

		for(auto evt : events)
		{
			((lemon_channel*)evt)->wait_recv(S,((lemon_actor*)S)->Id);
		}

		events.push_back(LEMON_CLOSE_CHANNEL_EVT);

		for(;;)
		{
			auto evt = lemon_wait(S,lock_t(lock),&events[0],events.size(),timeout);

			if(evt == LEMON_INVALID_HANDLE)
			{
				actor(S).check_throw();

				lemon_log_debug(S,"receiver waiting -- timeout");

				return nullptr;
			}
			else if(LEMON_CLOSE_CHANNEL_EVT == evt) continue;

			lemon_log_debug(S,"receiver waiting -- success");

			if(((lemon_channel*)evt)->pop(msg))
			{
				assert(msg.msg);

				assert(msg.sender != LEMON_INVALID_HANDLE);

				if((LEMON_SEND_NOBLOCK & ((lemon_channel*)evt)->flags()) == 0)
				{
					lemon_log_debug(S,"receiver notify ...");

					lemon_notify(S,msg.sender,&evt,1);

					lemon_log_debug(S,"receiver notify -- success");
				}

				return msg.msg;
			}
			else
			{
				((lemon_channel*)evt)->wait_recv(S,((lemon_actor*)S)->Id);
			}
		}
	}

	void lemon_channel_system::send(lemon_state S,lemon_channel_t id, void * block,size_t timeout)
	{
		typedef basic_lock<std::unique_lock<std::mutex>> lock_t;

		std::unique_lock<std::mutex> lock(_mutex);

		auto iter = _channels.find(id);

		if(_channels.end() == iter)
		{
			throw lemon_raise_errno(S,nullptr,LEMON_CHANNEL_NOT_EXISTS);
		}

		auto channel = iter->second;

		lemon_msg msg = {((lemon_actor*)S)->Id,block};

		channel->push(S,msg);

		channel->notif_one(S);

		if((LEMON_SEND_NOBLOCK & channel->flags()) == 0)
		{
			lemon_event_t evt[] = {(lemon_event_t)channel};

			lemon_log_debug(S,"sender waiting ...");

			lemon_wait(S,lock_t(lock),evt,sizeof(evt)/sizeof(lemon_event_t),timeout);

			if(actor(S).fail())
			{
				lemon_raise_trace_msg(S,"wait recv error !!!");
			}
		}
	}
}}