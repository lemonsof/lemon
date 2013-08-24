
#include <lemon/assembly.h>
#include <lemonxx/mutex.hpp>
#include <lemon/kernel/func.hpp>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/channel.hpp>



namespace lemon{namespace kernel{

	typedef basic_lock<std::unique_lock<std::mutex>> lock_wrapper;

	lemon_channel::lemon_channel(lemon_system* sysm,lemon_msg_close_f f,void * userdata)
		:_system(sysm)
		,_counter(1)
		,_f(f)
		,_userdata(userdata)
	{

	}

	void lemon_channel::close_msg(void *msg)
	{
		if(_f) _f((lemon_state)_system->main_actor(),msg);
	}

	lemon_push_channel::lemon_push_channel(lemon_system* sysm,size_t maxlen, lemon_msg_close_f f,void * userdata)
		:lemon_channel(sysm,f,userdata)
		,_length(maxlen)
	{

	}

	lemon_push_channel::~lemon_push_channel()
	{
		for(auto msg : _export)
		{
			close_msg(msg.userdata);

			if(msg.source != LEMON_INVALID_HANDLE(lemon_t))
			{
				lemon_event_t events[] = {close_event()};

				system()->notify(msg.source,cbuff(events));
			}
		}

		for(auto target : _import)
		{
			lemon_event_t events[] = {close_event()};

			system()->notify(target,cbuff(events));
		}
	}

	bool lemon_push_channel::send(lemon_actor * actor,void * data,int flags, size_t timeout)
	{
		if(flags != 0 && (LEMON_CHANNEL_IMPORT & flags))
		{
			lemon_log_error(system()->trace_system(),*actor,"the push channel import end not support send option");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		std::unique_lock<std::mutex> lock(_mutex);

		if(_export.size() == _length)
		{
			lemon_log_error(system()->trace_system(),*actor,"the max channel(%p) len(%u) error",this,(unsigned int)_length);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_MAX_CHANNEL_LENGTH);

			throw errorCode;
		}

		if(!_import.empty())
		{
			lemon_t target = _import.front();

			_import.pop_front();

			lemon_event_t events[] = {(lemon_event_t)this};

			system()->notify(target,cbuff(events));
		}

		lemon_msg msg = {LEMON_INVALID_HANDLE(lemon_t),data};

		if(timeout != 0)
		{
			msg.source = *actor;

			_export.push_back(msg);

			lemon_event_t events[] = {(lemon_event_t)this,close_event()};

			auto evt = wait(actor,lock_wrapper(lock),cbuff(events),timeout);

			if(evt == close_event())
			{
				lemon_log_error(system()->trace_system(),*actor,"actor(%p) waiting channel(%p) closing ",actor,this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_CHANNEL_NOT_EXISTS);

				throw errorCode;
			}

			if(evt != LEMON_INVALID_HANDLE(lemon_event_t)) return true;
		}
		else
		{
			_export.push_back(msg);
		}

		return false;
	}

	void* lemon_push_channel::recv(lemon_actor * actor, int flags, size_t timeout)
	{
		if(flags != 0 && (LEMON_CHANNEL_EXPORT & flags))
		{
			lemon_log_error(system()->trace_system(),*actor,"the push channel export end not support recv option");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		std::unique_lock<std::mutex> lock(_mutex);

		for(;;)
		{
			if(_export.empty())
			{
				if(timeout == 0) return nullptr;

				lemon_event_t events[] = {(lemon_event_t)this,close_event()};

				_import.push_back(*actor);

				auto evt = wait(actor,lock_wrapper(lock),cbuff(events),timeout);

				if(evt == close_event())
				{
					lemon_log_error(system()->trace_system(),*actor,"actor(%p) waiting channel(%p) closing ",actor,this);

					lemon_declare_errinfo(errorCode);

					lemon_user_errno(errorCode,LEMON_CHANNEL_NOT_EXISTS);

					throw errorCode;
				}

				if(evt == LEMON_INVALID_HANDLE(lemon_event_t)) return nullptr;

				continue;
			}

			lemon_msg msg = _export.front();

			_export.pop_front();

			if(msg.source != LEMON_INVALID_HANDLE(lemon_t))
			{
				lemon_event_t events[] = {(lemon_event_t)this};

				system()->notify(msg.source,cbuff(events));
			}

			return msg.userdata;
		}
	}
}}

