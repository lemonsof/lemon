
#include <lemon/assembly.h>
#include <lemonxx/mutex.hpp>
#include <lemon/kernel/func.hpp>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/channel.hpp>



namespace lemon{namespace kernel{

	typedef basic_lock<std::unique_lock<std::mutex>> lock_wrapper;

	lemon_channel::lemon_channel(lemon_system* sysm,lemon_msg_f f,void * userdata)
		:_system(sysm)
		,_counter(1)
		,_f(f)
		,_userdata(userdata)
	{

	}

	void lemon_channel::close_msg(void *msg)
	{
		if(_f) _f((lemon_state)_system->main_actor(),LEMON_MSG_CLOSE,msg);
	}

	void* lemon_channel::clone_msg(void *msg)
	{
		if (_f) return _f((lemon_state) _system->main_actor(), LEMON_MSG_CLONE, msg);

		return msg;
	}

	lemon_push_channel::lemon_push_channel(lemon_system* sysm,size_t maxlen, lemon_msg_f f,void * userdata)
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

				lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

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

					lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

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


	//////////////////////////////////////////////////////////////////////////////////////////


	lemon_publish_channel::lemon_publish_channel(lemon_system* sysm, size_t maxlen, lemon_msg_f f, void * userdata)
		: lemon_channel(sysm, f, userdata), _maxlen(maxlen), _current(LEMON_INVALID_HANDLE(lemon_t))
	{

	}

	lemon_publish_channel::~lemon_publish_channel()
	{
		for (auto msg : _import)
		{
			//close_msg(*msg.userdata);

			assert(msg.target != LEMON_INVALID_HANDLE(lemon_t));

			lemon_event_t events [] = { close_event() };

			system()->notify(msg.target, cbuff(events));
		}
	}

	bool lemon_publish_channel::send(lemon_actor * actor, void * msg, int flags, size_t timeout)
	{
		if (flags != 0 && (LEMON_CHANNEL_IMPORT & flags))
		{
			lemon_log_error(system()->trace_system(), *actor, "the publish channel import end not support send option");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		{
			std::unique_lock<std::mutex> lock(_mutex);

			if (_import.empty())
			{
				if (timeout == 0)
				{
					close_msg(msg);

					return false;
				}

				_current = *actor;

				lemon_event_t events [] = { (lemon_event_t)this, close_event() };

				auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

				_current = LEMON_INVALID_HANDLE(lemon_t);

				if (evt == close_event())
				{
					lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

					lemon_declare_errinfo(errorCode);

					lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

					throw errorCode;
				}

				if (evt == LEMON_INVALID_HANDLE(lemon_event_t))
				{
					close_msg(msg);

					return false;

				}
			}

			auto iter = _import.begin(), end = -- _import.end();

			lemon_event_t events [] = { (lemon_event_t)this };
			
			for (; iter != end; ++ iter)
			{
				if (system()->notify(iter->target, cbuff(events)))
				{
					*iter->userdata = clone_msg(msg);
				}
			}

			if (!system()->notify(iter->target, cbuff(events)))
			{
				close_msg(msg);
			}
			else
			{
				*iter->userdata = msg;
			}

			_import.clear();
		}

		return true;
	}

	void* lemon_publish_channel::recv(lemon_actor * actor, int flags, size_t timeout)
	{
		if (flags != 0 && (LEMON_CHANNEL_EXPORT & flags))
		{
			lemon_log_error(system()->trace_system(), *actor, "the publish channel export end not support recv option");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		std::unique_lock<std::mutex> lock(_mutex);

		if (_maxlen == _import.size())
		{
			lemon_log_error(system()->trace_system(), *actor, "the subscriber reach max length");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_MAX_CHANNEL_LENGTH);

			throw errorCode;
		}

		if (_current != LEMON_INVALID_HANDLE(lemon_t))
		{
			lemon_event_t events [] = { (lemon_event_t)this };

			system()->notify(_current, cbuff(events));
		}

		void* userdata = nullptr;

		lemon_msg msg = {*actor,&userdata};

		_import.push_back(msg);

		lemon_event_t events [] = { (lemon_event_t)this, close_event() };

		auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

		if (evt == close_event())
		{
			lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

			throw errorCode;
		}

		if(actor->killed())
		{
			if(userdata) close_msg(userdata);
		}

		if (evt == LEMON_INVALID_HANDLE(lemon_event_t))
		{
			
			if(userdata) close_msg(userdata);

			return nullptr;

		}

		return userdata;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	lemon_request_channel::lemon_request_channel(lemon_system* sysm, size_t maxlen, lemon_msg_f f, void * userdata)
		: lemon_channel(sysm, f, userdata), _maxlen(maxlen),_responder(LEMON_INVALID_HANDLE(lemon_t))
	{
		_currentMsg.source = LEMON_INVALID_HANDLE(lemon_t);
	}

	lemon_request_channel::~lemon_request_channel()
	{
		for(auto msg : _improt)
		{
			clone_msg(msg.userdata);

			assert(msg.source != LEMON_INVALID_HANDLE(lemon_t));

			lemon_event_t events [] = { close_event() };

			system()->notify(msg.source, cbuff(events));
		}

		for(auto msg : _response_map)
		{
			if(msg.second.userdata) clone_msg(msg.second.userdata);

			assert(msg.second.source != LEMON_INVALID_HANDLE(lemon_t));

			lemon_event_t events [] = { close_event() };

			system()->notify(msg.second.source, cbuff(events));
		}
	}

	bool lemon_request_channel::send(lemon_actor * actor, void * msg, int flags, size_t timeout)
	{
		if (flags == 0)
		{
			lemon_log_error(system()->trace_system(), *actor, "request channel must special the export/import flags");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		if (flags & LEMON_CHANNEL_EXPORT)
		{
			return send_export(actor,msg,flags,timeout);
		}
		else
		{
			return send_import(actor,msg,flags,timeout);
		}
	}

	void* lemon_request_channel::recv(lemon_actor * actor, int flags, size_t timeout)
	{
		if (flags == 0)
		{
			lemon_log_error(system()->trace_system(), *actor, "request channel must special the export/import flags");

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		if (flags & LEMON_CHANNEL_EXPORT)
		{
			return recv_export(actor, flags, timeout);
		}
		else
		{
			return recv_import(actor, flags, timeout);
		}
	}


	bool lemon_request_channel::send_import(lemon_actor * actor, void * msg, int /*flags*/, size_t timeout)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_response_map.find(*actor) != _response_map.end())
		{
			lemon_log_error(system()->trace_system(), *actor, "actor(%p) had pending request not receive response from channle(%p) ", actor, this);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		{
			lemon_request request = { _id++, *actor, msg };

			lemon_response response = { request.id,*actor, nullptr };

			_response_map[*actor] = response;

			_improt.push_back(request);
		}

		if (_responder != LEMON_INVALID_HANDLE(lemon_t))
		{
			lemon_event_t events [] = { (lemon_event_t)this };

			system()->notify(_responder, cbuff(events));
		}

		if (timeout != 0)
		{
			lemon_event_t events [] = { (lemon_event_t)this, close_event() };

			auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

			if (evt == close_event())
			{
				lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

				throw errorCode;
			}

			if (evt == LEMON_INVALID_HANDLE(lemon_event_t)) return false;

			return true;
		}

		return false;
	}

	void* lemon_request_channel::recv_import(lemon_actor * actor, int /*flags*/, size_t timeout)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		{
			auto iter = _response_map.find(*actor);

			if (iter == _response_map.end())
			{
				lemon_log_error(system()->trace_system(), *actor, "actor(%p) must send request first(%p) ", actor, this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

				throw errorCode;
			}

			if (iter != _response_map.end())
			{
				if (iter->second.userdata != nullptr)
				{
					void * userdata = iter->second.userdata;

					_response_map.erase(iter);

					return userdata;
				}
			}
		}

		if (timeout != 0)
		{
			lemon_event_t events [] = { (lemon_event_t)this, close_event() };

			auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

			if (evt == close_event())
			{
				lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

				throw errorCode;
			}

			if (evt == LEMON_INVALID_HANDLE(lemon_event_t)) return nullptr;

			auto iter = _response_map.find(*actor);

			assert(iter != _response_map.end());

			void * userdata = iter->second.userdata;

			_response_map.erase(iter);

			return userdata;
		}

		return nullptr;
	}

	bool lemon_request_channel::send_export(lemon_actor * actor, void * msg, int /*flags*/, size_t timeout)
	{
		if (_currentMsg.source == LEMON_INVALID_HANDLE(lemon_t))
		{
			lemon_log_error(system()->trace_system(), *actor, "request channle export actor(%p) must call recv first(%p) ", actor, this);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		std::unique_lock<std::mutex> lock(_mutex);

		if (_responder != LEMON_INVALID_HANDLE(lemon_t) && _responder != *actor)
		{
			lemon_log_error(system()->trace_system(), *actor, "request channle(%p) already bound export actor(%p) ", this,_responder);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		auto iter = _response_map.find(_currentMsg.source);

		_currentMsg.source = LEMON_INVALID_HANDLE(lemon_t);

		if (iter == _response_map.end() || _currentMsg.id != iter->second.id)
		{
			close_msg(msg);

			_currentMsg.source = LEMON_INVALID_HANDLE(lemon_t);

			return true;
		}

		iter->second.userdata = msg;

		lemon_event_t events [] = { (lemon_event_t)this };

		if (!system()->notify(iter->first, cbuff(events)))
		{
			lemon_event_t events [] = { (lemon_event_t)this, close_event() };

			auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

			if (evt == close_event())
			{
				lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

				throw errorCode;
			}

			if (evt == LEMON_INVALID_HANDLE(lemon_event_t)) return false;

			return true;
		}
		
		return true;
	}

	void* lemon_request_channel::recv_export(lemon_actor * actor, int /*flags*/, size_t timeout)
	{
		if (_currentMsg.source != LEMON_INVALID_HANDLE(lemon_t))
		{
			lemon_log_error(system()->trace_system(), *actor, "request channle export actor(%p) must response previous request first (%p) ", actor, this);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		std::unique_lock<std::mutex> lock(_mutex);

		if (_responder != LEMON_INVALID_HANDLE(lemon_t) && _responder != *actor)
		{
			lemon_log_error(system()->trace_system(), *actor, "request channle(%p) already bound export actor(%p) ", this, _responder);

			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode, LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		_responder = *actor;

		if (_improt.empty())
		{
			lemon_event_t events [] = { (lemon_event_t)this, close_event() };

			auto evt = wait(actor, lock_wrapper(lock), cbuff(events), timeout);

			if (evt == close_event())
			{
				lemon_log_error(system()->trace_system(), *actor, "actor(%p) waiting channel(%p) closing ", actor, this);

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode, LEMON_CHANNEL_CLOSING);

				throw errorCode;
			}

			if (evt == LEMON_INVALID_HANDLE(lemon_event_t)) nullptr;
		}

		assert(!_improt.empty());

		_currentMsg = _improt.front();

		_improt.pop_front();

		return _currentMsg.userdata;
	}
}}

