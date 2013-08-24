#include <lemon/assembly.h>
#include <lemon/kernel/actor.hpp>
#include <lemon/kernel/channel-system.hpp>

namespace lemon{namespace kernel{

	lemon_channel_system::lemon_channel_system()
	{

	}

	lemon_channel_system::~lemon_channel_system()
	{
	
	}

	void lemon_channel_system::stop()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		for(auto id : _channels)
		{
			lemon_channel::from(id)->release(true);
		}

		_channels.clear();
	}

	lemon_channel_t lemon_channel_system::make_channel(lemon_t /*source*/,int type,size_t maxlen,lemon_msg_close_f f,void * userdata)
	{
		lemon_channel * channel = nullptr;

		switch (type)
		{
		case LEMON_CHANNEL_PUSH:
			{
				channel = new lemon_push_channel(_system,maxlen,f,userdata);

				break;
			}
		case LEMON_CHANNEL_PUB:
			{
				break;
			}
		case LEMON_CHANNEL_REQ:
			{
				break;
			}
		default:
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

				throw errorCode;
			}
		}

		{
			std::unique_lock<std::mutex> lock(_mutex);

			_channels.insert(*channel);
		}

		return *channel;
	}

	void lemon_channel_system::close_channel(lemon_t /*source*/,lemon_channel_t id)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if(_channels.count(id))
		{
			if(lemon_channel::from(id)->release())
			{
				_channels.erase(id);
			}
		}
	}

	bool lemon_channel_system::send(lemon_t source,lemon_channel_t id,void * msg,int flags, size_t timeout)
	{
		lemon_channel *channel = lemon_channel::from(id);

		{
			std::unique_lock<std::mutex> lock(_mutex);

			if(!_channels.count(id))
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_CHANNEL_NOT_EXISTS);

				throw errorCode;
			}

			channel->addref();
		}

		bool status = channel->send(lemon_actor::from(source),msg,flags,timeout);

		close_channel(source,id);

		return status;
	}

	void* lemon_channel_system::recv(lemon_t source,lemon_channel_t id,int flags, size_t timeout)
	{
		lemon_channel *channel = lemon_channel::from(id);

		{
			std::unique_lock<std::mutex> lock(_mutex);

			if(!_channels.count(id))
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_CHANNEL_NOT_EXISTS);

				throw errorCode;
			}

			channel->addref();
		}


		auto data = channel->recv(lemon_actor::from(source),flags,timeout);

		close_channel(source,id);

		return data;
	}
}}