#include <new>
#include <assert.h>
#include <lemon/kernel/func.hpp>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/actor-system.hpp>

#ifndef WIN32
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif //

namespace lemon{namespace kernel{

#ifdef WIN32
		SYSTEM_INFO system_info_()
		{
			SYSTEM_INFO si;
			::GetSystemInfo( & si);
			return si;
		}

		inline SYSTEM_INFO system_info()
		{
			static SYSTEM_INFO si = system_info_();

			return si;
		}

		inline size_t pagesize(){ return (size_t)system_info().dwPageSize; }

		inline size_t page_count(size_t stacksize){ return (stacksize + pagesize()) / pagesize(); }

		inline void * __alloc(size_t stacksize)
		{
			size_t pages = page_count(stacksize) + 1;

			size_t newsize = pages * pagesize();

			void * limit = ::VirtualAlloc( 0, newsize, MEM_COMMIT, PAGE_READWRITE);

			if(!limit) return NULL;

			memset( limit,0,(int)newsize);

			DWORD old_options;

			::VirtualProtect(limit, pagesize(), PAGE_READWRITE | PAGE_NOACCESS, & old_options);

			return (char*)limit + newsize;
		}

		inline void __free(void* stack, size_t stacksize)
		{
			const size_t pages = page_count(stacksize) + 1;
			const size_t size_ = pages * pagesize();

			void * limit = static_cast< char * >( stack) - size_;

			::VirtualFree( limit, 0, MEM_RELEASE);
		}

#else

		inline size_t pagesize()
		{
			// conform to POSIX.1-2001
			static size_t size = ::sysconf( _SC_PAGESIZE);
			return size;
		}

		inline rlimit stacksize_limit_()
		{
			rlimit limit;
			// conforming to POSIX.1-2001
			const int result = ::getrlimit( RLIMIT_STACK, & limit);

			return limit;
		}

		inline rlimit stacksize_limit()
		{
			static rlimit limit = stacksize_limit_();
			return limit;
		}

		inline size_t page_count(size_t stacksize)
		{
			return (stacksize + pagesize()) / pagesize(); 
		}

		inline void * __alloc(size_t stacksize)
		{
			size_t pages = page_count(stacksize) + 1;

			size_t size_ = pages * pagesize();

			const int fd( ::open("/dev/zero", O_RDONLY) );

			// conform to POSIX.4 (POSIX.1b-1993, _POSIX_C_SOURCE=199309L)
			void * limit =
# if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
				::mmap( 0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
# else
				::mmap( 0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
# endif
			::close( fd);
			if ( ! limit) return NULL;

			memset( limit, 0,size_);

			// conforming to POSIX.1-2001
			const int result( ::mprotect( limit, pagesize(), PROT_NONE) );

			return static_cast< char * >( limit) + size_;
		}

		inline void __free(void* stack, size_t stacksize)
		{
			const size_t pages = page_count( stacksize) + 1;

			const size_t size_ = pages * pagesize();


			void * limit = static_cast< char * >(stack) - size_;
			// conform to POSIX.4 (POSIX.1b-1993, _POSIX_C_SOURCE=199309L)
			::munmap( limit, size_);
		}

#endif //WIN32

		void __f(lemon_actor * self)
		{
			assert((lemon_context_t*)self);

			self->call();

			self->exit();

			lemon_context_jump(*self,self->parent(),0);
		}


		lemon_actor* lemon_actor_system::create(lemon_t source,lemon_f f, void * userdata,size_t stacksize)
		{
			void * block = __alloc(stacksize + sizeof(lemon_actor));

			if(!block)
			{
				lemon_log_error(_sysm->trace_system(),source,"create new actor failed,insufficient memory");

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}

			block = (lemon_byte_t*)block - sizeof(lemon_actor);

			lemon_context_t * context = lemon_make_context(block,stacksize,(void(*)(intptr_t))&__f);

			return new(block) lemon_actor(_sysm,context,f,userdata);
		}

		void lemon_actor_system::close(lemon_actor* actor)
		{
			void * block  = (lemon_byte_t*)actor + sizeof(lemon_actor);

			__free(block,((lemon_context_t*)*actor)->fc_stack.size + sizeof(lemon_actor));
		}


		//////////////////////////////////////////////////////////////////////////

		lemon_man_runq::lemon_man_runq()
			:_exit(false)
			,_sysm(nullptr)
			,_mainActor(nullptr,&_mainContext,nullptr,nullptr)
		{
			
		}

		lemon_man_runq::~lemon_man_runq()
		{
			__free(_context->fc_stack.sp,_context->fc_stack.size);
		}

		void lemon_man_runq::start(lemon_system * sysm,size_t stacksize)
		{

			_sysm = sysm;

			void * block = __alloc(stacksize);

			if(!block)
			{
				lemon_log_error(_sysm->trace_system(),LEMON_INVALID_HANDLE(lemon_t),"create new actor failed,insufficient memory");

				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}

			_context = lemon_make_context(block,stacksize,(void(*)(intptr_t))&lemon_man_runq::__f);

			_mainActor.set_system(_sysm);

			_mainActor.parent_reset(_context);
		}

		void lemon_man_runq::proc()
		{
			_sysm->wait_or_kill(&_mainActor);

			for(;;)
			{
				lemon_actor * actor = _sysm->dispatch_one();

				if(!actor) break;

				actor->lock();

				actor->parent_reset(_context);

				lemon_context_jump(_context,*actor,(intptr_t)actor);

				_sysm->wait_or_kill(actor);
			}

			{
				std::unique_lock<std::mutex> lock(_mutex);

				while(!_exit) 
				{
					_condition.wait(lock);
				}
			}

			lemon_context_jump(_context,&_mainContext,(intptr_t)nullptr);
		}

		void lemon_man_runq::notify()
		{
			std::unique_lock<std::mutex> lock(_mutex);

			_exit = true;

			_condition.notify_one();
		}

		void lemon_man_runq::join()
		{
			if(_mainActor.parent() != _context)
			{
				lemon_context_jump(_mainActor,_mainActor.parent(),0);
			}
		}
}}