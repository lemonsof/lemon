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
#elif defined(LINUX) || defined(SOLARIS) || defined(AIX)  
HELIX_PRIVATE int hardware_concurrency(){

	return get_nprocs();
}
#else
#error "unsupported os"
#endif 


HELIX_API helix_t helix_open(helix_errcode * errorCode){
	
	helix_reset_errorcode(*errorCode);

	int cpus = hardware_concurrency() + 1;

	helix_kernel_t kernel = (helix_kernel_t)malloc(HELIX_HANDLE_SIZEOF(helix_kernel_t));

	if (kernel == NULL){
		helix_user_errno(*errorCode, HELIX_RESOURCE_ERROR)
	} else {

		kernel->thread_list = (HELIX_HANDLE_STRUCT_NAME(helix_t)*)malloc(HELIX_HANDLE_SIZEOF(helix_t) * cpus);

		kernel->thread_list_counter = cpus;

		init_main_runq(kernel, &kernel->thread_list[0], errorCode);

		if (helix_failed(*errorCode)){
			goto Error;
		}

		for (size_t i = 1; i < kernel->thread_list_counter; ++i){
			
			init_runq(kernel, &kernel->thread_list[i], errorCode);
			
			if (helix_failed(*errorCode)){
				goto Error;
			}
		}
	}

	return &kernel->thread_list[0];

Error:
	//make sure the man thread's helix_kenerl filed not null
	kernel->thread_list[1].helix_kenerl = kernel;

	helix_close(&kernel->thread_list[1]);

	return NULL;
}

HELIX_API void helix_close(helix_t helix){

	helix_kernel_t kernel = helix->helix_kenerl;

	assert(helix == &kernel->thread_list[0] && "only man thread can close the helix system");

	for (size_t i = 1; i < kernel->thread_list_counter; ++i){

		close_thread_and_join(&kernel->thread_list[i]);
	}

	free(kernel->thread_list);

	free(kernel);
}