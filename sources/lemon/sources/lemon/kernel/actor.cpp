#include <lemon/kernel/runq.hpp>
#include <lemon/kernel/actor.hpp>

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

namespace lemon{namespace impl{

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
		assert(self->Context);

		try
		{
			self->F((lemon_state)self,self->Userdata);
		}
		catch(...)
		{

		}

		self->Exit |= (int)exit_status::exited;

		lemon_context_jump(self->Context,self->Q->context(),0);
	}

	lemon_actor::lemon_actor()
		:Id(LEMON_MAIN_ACTOR_ID)
		,Status(actor_status::running)
		,F(nullptr)
		,Userdata(nullptr)
		,Context(nullptr)
		,Exit(0)
	{
		lemon_reset_errorinfo(LastError);
	}

	lemon_actor::lemon_actor(lemon_system * system,lemon_t id,lemon_f f, void * userdata,lemon_context_t* context)
		:Id(id)
		,Status(actor_status::init)
		,F(f)
		,Userdata(userdata)
		,System(system)
		,Context(context)
		,Exit(0)
	{
		lemon_reset_errorinfo(LastError);
	}

	lemon_actor * lemon_actor_factory::create(lemon_state S,lemon_t id,lemon_f f, void * userdata,size_t stacksize)
	{
		void * block = __alloc(stacksize + sizeof(lemon_actor));

		if(!block)
		{
			throw lemon_raise_errno(S,NULL,LEMON_RESOURCE_ERROR);
		}

		block = (lemon_byte_t*)block - sizeof(lemon_actor);

		lemon_context_t * context = lemon_make_context(block,stacksize,(void(*)(intptr_t))&__f);

		return new (block) lemon_actor(((lemon_actor*)S)->System,id,f,userdata,context);
	}

	void lemon_actor_factory::close(lemon_actor * actor)
	{
		assert(actor->Context);

		void * block  = (lemon_byte_t*)actor + sizeof(lemon_actor);

		__free(block,actor->Context->fc_stack.size + sizeof(lemon_actor));
	}
}}