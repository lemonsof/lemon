/**
* 
* @file     actor
* @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_ACTOR_HPP
#define HELIX_ACTOR_HPP
#include <helix/stack.hpp>
#include <helix/object.hpp>
#include <helix/context/fcontext.h>
namespace helix{ namespace impl{

	class runtimes;

	class basic_runq;

	class main_runq;

	enum class actor_status
	{
		sleeping,running,stopped
	};

	enum class actor_event_status
	{
		null,set,unset
	};

	struct actor_event_t : public helix_event
	{
		actor_event_status status;
	};

	class basic_actor_t : public object
	{
	public:

		basic_actor_t();

		virtual ~basic_actor_t(){}

		runtimes* rt(){ return _runtimes; }

		helix_context_t* context(){ return _context; }

		basic_runq * Q(){ return _runq; }

		void Q(basic_runq* q) { _runq = q; }

		actor_status status() const { return _status; }

		void status(actor_status s){_status = s;}

		bool notify(uintptr_t eventid);

		bool poll_first_event();

		helix_event * get_event(){ return _current_event; }

		void add_event(helix_event * event);

		helix_event* remove_event(uintptr_t eventid);

		void clear_events();

		uintptr_t handle() const { return _handle; }

		bool exited(){ return _exited;}

		helix_errcode * lasterror() 
		{
			if(helix_success(_lasterror))
			{
				return nullptr;
			}

			return &_lasterror; 
		}

		void lasterror(const helix_errcode& e){_lasterror = e;}

		void reset_lasterror()
		{
			helix_reset_errorcode(_lasterror);
		}

	protected:
		
		void exit(){ _exited = true; }

		void handle(uintptr_t h){_handle = h; }

		void rt(runtimes * r){_runtimes = r;}

		void context(helix_context_t* ctx){ _context = ctx; }

	private:
		bool									_exited;
		uintptr_t								_handle;
		actor_status							_status;
		basic_runq								*_runq;
		runtimes								*_runtimes;
		helix_context_t							*_context;
		helix_event								*_current_event;
		actor_event_t							_events[HELIX_MAX_EVENTS];
		helix_errcode							_lasterror;
	};

	class actor_t : public basic_actor_t
	{
	public:
		actor_t(runtimes * rt,void(*f)(helix_t,void*),void * userdata,size_t stacksize,uintptr_t handle);

		~actor_t();
		
		void call();

	private:
		void									(*_f)(helix_t,void*);
		void									*_userdata;
		basic_stack								_stack;
	};

	class main_actor_t : public basic_actor_t
	{
	public:
		void start(main_runq * Q);
	private:
		helix_context_t							_context;
	};
} }

#endif	//HELIX_ACTOR_HPP