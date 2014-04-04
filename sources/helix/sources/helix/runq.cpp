#include <cassert>
#include <functional>
#include <helix/runq.hpp>
#include <helix/runtimes.hpp>

namespace helix{ namespace impl{

	basic_runq::basic_runq()
		:_status(true)
	{

	}

	void basic_runq::dispatch_one(basic_actor_t * actor)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		actor->Q(this);

		_runq.push(actor);

		_condition.notify_one();
	}

	void basic_runq::dispatch_loop()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		while(_status || !_runq.empty())
		{
			if(_runq.empty())
			{
				_condition.wait(lock);
				continue;
			}

			basic_actor_t* actor = _runq.front();

			_runq.pop();

			lock.unlock();

			assert(actor != nullptr);

			helix_context_jump(&_context,actor->context(),(intptr_t)actor);

			_runtimes->sleep(actor);

			lock.lock();
		}
	}

	void basic_runq::context(helix_context_t * ctx)
	{
		memcpy(&_context,ctx,sizeof(helix_context_t));
	}

	void basic_runq::exit()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		_status = false;

		_condition.notify_one();
	}

	runq::runq(runtimes * rt)
	{
		this->rt(rt);
		_thread = std::thread(std::bind(&runq::dispatch_loop,this));
	}

	void main_runq::dispatch_loop(basic_actor_t * actor)
	{
		helix_context_jump(actor->Q()->context(),actor->context(),0);

		actor->rt()->sleep(actor);

		actor->Q()->dispatch_loop();

		helix_context_jump(actor->Q()->context(),actor->context(),0);
	}

	void main_runq::start(runtimes * rt)
	{
		this->rt(rt);
		
		_stack.create(HELIX_STACK_SIZE,rt->alloc());

		helix_context_t * context = helix_make_context(_stack,_stack.size(),(void(*)(intptr_t))&main_runq::dispatch_loop);

		this->context(context);

		rt->main_actor()->start(this);

		helix_context_jump(rt->main_actor()->context(),this->context(),(intptr_t)rt->main_actor());
	}


}}