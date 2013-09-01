#include <lemon/kernel/actor.hpp>
#include <lemon/kernel/system.hpp>

namespace lemon{namespace kernel{

	lemon_actor::lemon_actor()
		:_killed(false)
		,_exited(false)
		,_system(nullptr)
		,_context(nullptr)
		,_f(nullptr)
		,_userdata(nullptr)
		,_mutex(nullptr)
	{
		events_reset();

		timeout_reset();

		notified_reset();

		lasterror_reset();
	}

	lemon_actor::lemon_actor(lemon_system * sysm,lemon_context_t * context,lemon_f f, void * userdata)
		:_killed(false)
		,_exited(false)
		,_system(sysm)
		,_context(context)
		,_f(f)
		,_userdata(userdata)
		,_mutex(nullptr)
	{
		events_reset();

		timeout_reset();

		notified_reset();

		lasterror_reset();
	}

	bool lemon_actor::killed() const
	{
		return _system->exited() || _killed;
	}
}}

