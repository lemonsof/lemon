/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/21
*/
#ifndef LEMON_ACTOR_HPP
#define LEMON_ACTOR_HPP
#include <assert.h>
#include <lemon/abi.h>
#include <lemonxx/buffer.hpp>
#include <lemonxx/nocopyable.hpp>
#include <lemon/context/fcontext.h>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_runq;

	class lemon_actor : private nocopyable
	{
	public:

		typedef const_buff<lemon_event_t>			events_t;

		static lemon_actor* from(lemon_t id)
		{
			return reinterpret_cast<lemon_actor*>(id);
		}

		lemon_actor();

		lemon_actor(lemon_system * sysm,lemon_context_t * context,lemon_f f, void * userdata);

		operator lemon_t() const { return (lemon_t)this; }

		operator lemon_context_t* () const { return _context; }

		lemon_context_t* get_context() { return _context; }

		void set_system(lemon_system * sysm){ _system = sysm; }

		lemon_system* get_system() { return _system; }

		void mutex_reset(const lemon_mutext_t * mutex)
		{
			_mutex = mutex;
		}

		void mutex_reset()
		{
			_mutex = nullptr;
		}

		void unlock()
		{
			if(_mutex)
			{
				_mutex->unlock(_mutex->mutex);
			}
		}


		void lock()
		{
			if(_mutex)
			{
				_mutex->lock(_mutex->mutex);
			}
		}

		void events_reset(const events_t & events){ _events = events; }

		void events_reset() { reset(_events); }

		const events_t & events() const { return _events; }

		size_t timeout() const { return _timeout; }

		void timeout_reset(size_t tt) { _timeout = tt; }

		void timeout_reset() { _timeout = lemon_infinite; }

		const lemon_errno_info & lasterror() const { return _lasterror; }

		void lasterror_reset(const lemon_errno_info & info)
		{
			_lasterror = info;
		}

		void lasterror_reset(uintptr_t code,const lemon_uuid_t * catalog,const char * file,int lines)
		{
			_lasterror.error.code= code;
			
			_lasterror.error.catalog = catalog;

			_lasterror.file = file;

			_lasterror.lines = lines;
		}


		void lasterror_reset()
		{
			lemon_reset_errorinfo(_lasterror);
		}

		bool failed() const
		{
			return lemon_failed(_lasterror);
		}

		lemon_context_t* parent() const { return _parent; }

		void parent_reset() { _parent = nullptr; }

		void parent_reset(lemon_context_t * ctx) { _parent = ctx;}

		void kill()
		{
			_killed = true;
		}

		bool killed() const;

		void exit()
		{
			_exited = true;
		}

		bool exited() const { return _exited; } 

		void call()
		{
			_f((lemon_state)this,_userdata);
		}

		void notified_reset()
		{
			_notified = LEMON_INVALID_HANDLE(size_t);
		}

		void notified_reset(size_t index)
		{
			assert(index < _events.length);

			_notified = index;
		}

		lemon_event_t notified() const
		{
			if(_notified == LEMON_INVALID_HANDLE(size_t))
			{
				return LEMON_INVALID_HANDLE(lemon_event_t);
			}

			return _events.buff[_notified];
		}

	private:

		bool										_killed;

		bool										_exited;

		lemon_system								*_system;

		lemon_context_t								*_parent;

		lemon_context_t								*_context;
		
		lemon_f										_f;

		void										*_userdata;

		const lemon_mutext_t						*_mutex;

		events_t									_events;

		size_t										_notified;

		size_t										_timeout;

		lemon_errno_info							_lasterror;
	};

}}

#endif //LEMON_ACTOR_HPP