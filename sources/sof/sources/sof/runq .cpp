#include <iostream>
#include <functional>
#include <sof/runq.hpp>
#include <sof/system.hpp>

namespace sof{namespace impl{
	
	void basic_sof_runq::stop()
	{
		sysm()->stop();
	}

	const sof_errno_info* basic_sof_runq::last_errno() const
	{
		if(SOF_SUCCESS(_lasterror)) return nullptr;

		return &_lasterror;
	}

	void basic_sof_runq::raise_error(const char* msg ,const sof_errno_info* info)
	{
		assert(get_current());

		_lasterror = *info;

		const sof_byte_t * bytes = (const sof_byte_t *)_lasterror.error.catalog;

		if(msg)
		{
			sof_log_error((sof_state)get_current(),"raise exception: %s\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
				msg,
				_lasterror.error.code,
				bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
				bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
				_lasterror.file,
				_lasterror.lines);
		}
		else
		{
			sof_log_error((sof_state)get_current(),"raise exception: \n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
				_lasterror.error.code,
				bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
				bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
				_lasterror.file,
				_lasterror.lines);
		}

		
	}

	void basic_sof_runq::raise_trace(const char * file, int lines)
	{
		assert(get_current());

		const sof_byte_t * bytes = (const sof_byte_t *)_lasterror.error.catalog;

		sof_log_error((sof_state)get_current(),"trace exception\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
			_lasterror.error.code,
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
			file,
			lines);
	}

	void basic_sof_runq::reset_errno()
	{
		SOF_RESET_ERRORINFO(_lasterror);
	}

	void basic_sof_runq::trace( dtrace::level level,const char * fmt,va_list arg)
	{
		_system->trace_system().trace(id(),level,fmt,arg);
	}

	sof_t basic_sof_runq::go(sof_f f, void * userdata,size_t stacksize)
	{
		return sysm()->go((sof_state)get_current(),f,userdata,stacksize);
	}

	void basic_sof_runq::dispatch()
	{
		for(;;)
		{
			sof_actor * actor = sysm()->dispatch();

			if(!actor) break;

			set_current(actor);

			if(actor->Mutex.mutex) actor->Mutex.lock(actor->Mutex.mutex);

			sof_context_jump(context(),actor->Context,(intptr_t)actor);

			sysm()->wait_or_kill(actor);
		}
	}

	bool basic_sof_runq::notify(sof_t target, const sof_event_t * waitlist, size_t len)
	{
		return sysm()->notify(target,waitlist,len);
	}

	sof_event_t basic_sof_runq::wait(const sof_mutext_t * mutex,const sof_event_t * waitlist, size_t len)
	{
		if(get_current()->Id == SOF_MAIN_ACTOR_ID)
		{
			error_info errorCode;

			SOF_USER_ERROR(errorCode,SOF_UNSUPPORT_OPTION);

			errorCode.raise((sof_state)get_current());
		}

		if(get_current()->Exit & (int)exit_status::killed)
		{
			error_info errorCode;

			SOF_USER_ERROR(errorCode,SOF_KILLED);

			errorCode.raise((sof_state)get_current());
		}

		if(mutex) get_current()->Mutex = *mutex;

		get_current()->WaitList = waitlist;

		get_current()->Waits = len;

		get_current()->WaitResult = SOF_INVALID_HANDLE;

		sof_context_jump(get_current()->Context,context(),0);

		if(get_current()->Exit & (int)exit_status::killed)
		{
			error_info errorCode;

			SOF_USER_ERROR(errorCode,SOF_KILLED);

			errorCode.raise((sof_state)get_current());
		}

		get_current()->Mutex.mutex = nullptr;

		get_current()->WaitList = nullptr;

		get_current()->Waits = 0;

		return get_current()->WaitResult;
	}

	//////////////////////////////////////////////////////////////////////////

	main_sof_runq::main_sof_runq(size_t maxcoros,size_t maxchannels)
		:basic_sof_runq(new sof_system(maxcoros,maxchannels))
	{
		set_current(&_mainActor);
	}

	main_sof_runq::~main_sof_runq()
	{
		stop();

		join();

		delete sysm();
	}

	void main_sof_runq::join()
	{
		dispatch();

		sysm()->join();

		for(auto a : sysm()->actors())
		{
			auto actor = a.second;

			set_current(actor);

			actor->Exit |= (int)exit_status::killed;

			if(actor->Mutex.mutex) actor->Mutex.lock(actor->Mutex.mutex);

			sof_context_jump(context(),actor->Context,(intptr_t)actor);

			assert(actor->Exit & (int)exit_status::exited);

			sysm()->wait_or_kill(actor);
		}

		set_current(&_mainActor);
	}

	//////////////////////////////////////////////////////////////////////////


	sof_runq::sof_runq(sof_system * system) 
		: basic_sof_runq(system)
		, _worker(std::bind(&sof_runq::proc,this))
	{
		
	}

	sof_runq::~sof_runq()
	{
		join();
	}

	void sof_runq::join()
	{
		if(_worker.joinable()) _worker.join();
	}

	void sof_runq::proc()
	{
		dispatch();
	}

}}