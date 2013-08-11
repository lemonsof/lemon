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

	void basic_sof_runq::raise_error(sof_state S,const char* msg ,const sof_errno_info* info)
	{
		_lasterror = *info;

		const sof_byte_t * bytes = (const sof_byte_t *)_lasterror.error.catalog;

		if(msg)
		{
			sof_log_error(S,"raise exception: %s\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
				msg,
				_lasterror.error.code,
				bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
				bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
				_lasterror.file,
				_lasterror.lines);
		}
		else
		{
			sof_log_error(S,"raise exception: \n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
				_lasterror.error.code,
				bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
				bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
				_lasterror.file,
				_lasterror.lines);
		}

		
	}

	void basic_sof_runq::raise_trace(sof_state S,const char * file, int lines)
	{
		const sof_byte_t * bytes = (const sof_byte_t *)_lasterror.error.catalog;

		sof_log_error(S,"trace exception\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
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


	//////////////////////////////////////////////////////////////////////////

	main_sof_runq::main_sof_runq(size_t maxcoros,size_t maxchannels)
		:basic_sof_runq(new sof_system(maxcoros,maxchannels))
	{

	}

	main_sof_runq::~main_sof_runq()
	{
		delete sysm();
	}

	void main_sof_runq::join()
	{
		sysm()->join();
	}


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
		for(;;)
		{
			sof_actor * actor = sysm()->dispatch();

			if(!actor) break;
		}
	}

}}