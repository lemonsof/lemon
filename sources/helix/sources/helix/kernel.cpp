#include <assert.h>
#include <helix/kenerl.h>
#include <helix/assembly.h>
#include <helix/runq.h>
#ifdef WIN32
HELIX_PRIVATE int hardware_concurrency(){

	SYSTEM_INFO sysInfo;

	GetNativeSystemInfo(&sysInfo);

	return sysInfo.dwNumberOfProcessors;
}

typedef struct {
	helix_alloc_t alloc;
}									helix_win_alloc;

HELIX_PRIVATE void * __alloc(struct helix_alloc_t*, void * ptr, size_t, size_t nsize){
	if (nsize == 0) {
		free(ptr);
		return NULL;
	}
	else
		return realloc(ptr, nsize);
}

helix_win_alloc default_alloc = { { __alloc } };

#elif defined(LINUX) || defined(SOLARIS) || defined(AIX)  
HELIX_PRIVATE int hardware_concurrency(){

	return get_nprocs();
}
#else
#error "unsupported os"
#endif 



HELIX_API helix_t helix_open(helix_alloc_t* alloc,helix_errcode * errorCode){
	
	helix_reset_errorcode(*errorCode);

	if (alloc == NULL){
		alloc = &default_alloc.alloc;
	}

	int cpus = hardware_concurrency();

	size_t blocksize = HELIX_HANDLE_SIZEOF(helix_kernel_t) + HELIX_STACK_SIZE;

	void * block = helix_alloc(alloc, blocksize);

	if (block == NULL){
		helix_user_errno(*errorCode, HELIX_RESOURCE_ERROR);
		return NULL;
	}

	memset(block, 0, blocksize);

	block = (helix_byte_t*)block + HELIX_STACK_SIZE;

	helix_kernel_t kernel = (helix_kernel_t)block;

	kernel->alloc = alloc;

	kernel->thread_list = (HELIX_HANDLE_STRUCT_NAME(helix_t)*)helix_alloc(alloc,HELIX_HANDLE_SIZEOF(helix_t) * cpus);

	kernel->thread_list_counter = cpus;

	init_main_runq(kernel, &kernel->main_thread, errorCode);

	if (helix_failed(*errorCode)){
		goto Error;
	}

	for (size_t i = 0; i < kernel->thread_list_counter; ++i){

		init_runq(kernel, &kernel->thread_list[i], errorCode);

		if (helix_failed(*errorCode)){
			goto Error;
		}
	}
	return &kernel->main_thread;

Error:
	//make sure the man thread's helix_kenerl filed not null
	kernel->main_thread.helix_kenerl = kernel;

	helix_close(&kernel->main_thread);

	return NULL;
}

HELIX_API void helix_close(helix_t helix){

	helix_kernel_t kernel = helix->helix_kenerl;

	assert(helix == &kernel->main_thread && "only man thread can close the helix system");

	for (size_t i = 0; i < kernel->thread_list_counter; ++i){

		close_thread_and_join(&kernel->thread_list[i]);
	}

	helix_free(kernel->alloc,kernel->thread_list);

	void * block = (helix_byte_t*)kernel - HELIX_STACK_SIZE;

	helix_free(kernel->alloc,block);
}