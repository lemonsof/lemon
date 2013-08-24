#include <lemon/kernel/runq.hpp>
#include <lemon/kernel/system.hpp>

namespace lemon{namespace kernel{

	lemon_runq::lemon_runq(lemon_system * sysm):_sysm(sysm)
	{
		_worker = std::thread(&lemon_runq::proc,this);
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
		for(;;)
		{
			lemon_actor * actor = _sysm->dispatch_one();

			if(!actor) break; // exit the runq

			actor->lock();

			actor->parent_reset(&_context);

			lemon_context_jump(&_context,*actor,(intptr_t)actor);

			_sysm->wait_or_kill(actor);
		}
	}
}}