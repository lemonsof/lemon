#include <helix/runq.hpp>
#include <helix/actor.hpp>
#include <helix/assembly.h>
#include <helix/runtimes.hpp>

namespace helix{ namespace impl{

	basic_actor_t::basic_actor_t() :_exited(false)
	{
		_events[0].status = actor_event_status::unset;

		_events[0].id = timewheel_t::event();
	}

	bool basic_actor_t::notify(uintptr_t eventid)
	{
		for(actor_event_t &event:_events)
		{
			if(event.status != actor_event_status::null && event.id == eventid)
			{
				event.status = actor_event_status::set;
				return true;
			}
		}

		return false;
	}

	bool basic_actor_t::poll_first_event()
	{
		assert(actor_status::sleeping == _status && "only sleeping actor can call this function");

		for (actor_event_t &event : _events)
		{
			if (event.status == actor_event_status::set)
			{
				_current_event = &event;

				event.status = actor_event_status::unset;

				_status = actor_status::running;

				return true;
			}
		}

		return false;
	}

	void basic_actor_t::add_event(helix_event * source)
	{
		actor_event_t * last_null = nullptr;

		for (actor_event_t &event : _events)
		{
			if (event.status != actor_event_status::null)
			{
				if(event.id == source->id)
				{
					event.data = source->data;
					return;
				}
			}
			else
			{
				last_null = &event;
			}
		}

		if(last_null == nullptr)
		{
			helix_declare_errcode(errorCode);

			helix_user_errno(errorCode,HELIX_RESOURCE_ERROR);

			throw errorCode;
		}

		last_null->status = actor_event_status::unset;

		last_null->data = source->data;

		last_null->id = source->id;
	}

	helix_event* basic_actor_t::remove_event(uintptr_t eventid)
	{
		for (actor_event_t &event : _events)
		{
			if (event.status != actor_event_status::null && event.id == eventid)
			{
				event.status = actor_event_status::null;
				return &event;
			}
		}

		return nullptr;
	}

	void basic_actor_t::clear_events()
	{
		for (actor_event_t &event : _events)
		{
			event.status = actor_event_status::null;
		}

		_events[0].status = actor_event_status::unset;

		_events[0].id = timewheel_t::event();
	}

	
	void __f(actor_t * actor)
	{
		actor->call();
	}

	actor_t::actor_t(runtimes * rt,void(*f)(helix_t,void*),void * userdata,size_t stacksize,uintptr_t handle)
		:_f(f),_userdata(userdata)
	{
		basic_actor_t::rt(rt);
		_stack.create(stacksize,rt->alloc());
		basic_actor_t::context(helix_make_context(_stack,_stack.size(),(void(*)(intptr_t))__f));
		basic_actor_t::handle(handle);
	}

	actor_t::~actor_t()
	{
		_stack.release(rt()->alloc());
	}

	void actor_t::call()
	{
		_f((helix_t)this, _userdata);

		exit();
		//jump to dispatch process
		helix_context_jump(context(),Q()->context() , 0);
	}

	void main_actor_t::start(main_runq * Q)
	{
		basic_actor_t::rt(Q->rt());
		basic_actor_t::Q(Q);
		basic_actor_t::context(&_context);
		basic_actor_t::handle(0);
	}

} }