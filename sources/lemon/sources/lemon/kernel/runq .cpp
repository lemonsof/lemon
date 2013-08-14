#include <iostream>
#include <functional>
#include <lemon/kernel/runq.hpp>
#include <lemon/kernel/system.hpp>

namespace lemon{namespace impl{

	void basic_lemon_runq::dispatch()
	{
		for(;;)
		{
			lemon_actor * actor = sysm()->dispatch();

			if(!actor) break;

			set_current(actor);

			if(actor->Mutex.mutex) actor->Mutex.lock(actor->Mutex.mutex);

			lemon_context_jump(context(),actor->Context,(intptr_t)actor);

			sysm()->wait_or_kill(actor);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	main_lemon_runq::main_lemon_runq(size_t maxcoros,size_t maxchannels)
		:basic_lemon_runq(new lemon_system(maxcoros,maxchannels))
	{
		_mainActor.System = sysm();

		set_current(&_mainActor);
	}

	main_lemon_runq::~main_lemon_runq()
	{
		sysm()->stop();

		join();

		delete sysm();
	}

	void main_lemon_runq::join()
	{
		dispatch();

		sysm()->join();

		sysm()->kill_dispatch(this);

		set_current(&_mainActor);
	}

	//////////////////////////////////////////////////////////////////////////


	lemon_runq::lemon_runq(lemon_system * system) 
		: basic_lemon_runq(system)
		, _worker(std::bind(&lemon_runq::proc,this))
	{
		
	}

	lemon_runq::~lemon_runq()
	{
		join();
	}

	void lemon_runq::join()
	{
		if(_worker.joinable()) _worker.join();
	}

	void lemon_runq::proc()
	{
		dispatch();
	}

}}