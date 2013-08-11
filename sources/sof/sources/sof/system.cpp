#include <cassert>
#include <sof/system.hpp>

namespace sof{namespace impl{

	sof_system::sof_system(size_t maxcoros,size_t maxchannels)
		:_exit(true)
		,_maxcoros(maxcoros?maxcoros:SOF_MAX_COROS)
		,_maxchannels(maxchannels?maxchannels:SOF_MAX_CHANNELS)
	{
		size_t Qs = std::thread::hardware_concurrency();

		for(size_t i = 0; i < Qs; ++ i)
		{
			_runqs.push_back(new sof_runq(this));
		}
	}

	sof_system::~sof_system()
	{
		join();

		for(auto q : _runqs)
		{
			delete q;
		}
	}

	void sof_system::join() const
	{
		for(auto q : _runqs)
		{
			q->join();
		}
	}

	void sof_system::stop() 
	{
		std::unique_lock<std::mutex> lock(_mutex);

		_exit = true;

		_condition.notify_all();
	}

	sof_actor* sof_system::dispatch()
	{
		sof_actor* actor = nullptr;

		std::unique_lock<std::mutex> lock(_mutex);

		while(!_exit)
		{
			if(_activeActors.empty())
			{
				_condition.wait(lock);

				continue;
			}

			//pop the actor;

			actor = _activeActors.front();

			_activeActors.pop();

			return actor;
		}

		return actor;
	}
}}

