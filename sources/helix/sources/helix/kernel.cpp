#include <assert.h>
#include <helix/runq.h>
#include <helix/stack.h>
#include <helix/kernel.h>
#include <helix/utility.h>
#include <helix/assembly.h>

HELIX_API helix_t helix_open(helix_alloc_t* alloc,helix_errcode * errorCode){
	
	helix_reset_errorcode(*errorCode);

	if (alloc == NULL){
		alloc = get_default_alloc();
	}

	int cpus = hardware_concurrency();

	helix_kernel_t kernel = (helix_kernel_t)alloc_stack(alloc, HELIX_STACK_SIZE, HELIX_HANDLE_SIZEOF(helix_kernel_t));

	if (kernel == NULL){
		helix_user_errno(*errorCode,HELIX_RESOURCE_ERROR);

		return NULL;
	}

	kernel->alloc = alloc;

	kernel->thread_list = (HELIX_HANDLE_STRUCT_NAME(helix_runq_t)*)helix_alloc(alloc,HELIX_HANDLE_SIZEOF(helix_runq_t) * cpus);

	kernel->thread_list_counter = cpus;

	kernel->main_actor.runq = &kernel->main_runq;

	init_main_runq(kernel, &kernel->main_runq);

	for (size_t i = 0; i < kernel->thread_list_counter; ++i){

		init_runq(kernel, &kernel->thread_list[i]);
	}

	return &kernel->main_actor;
}

HELIX_API void helix_close(helix_t helix){

	helix_kernel_t kernel = helix->runq->helix_kenerl;

	assert(helix == &kernel->main_actor && "only man thread can close the helix system");

	for (size_t i = 0; i < kernel->thread_list_counter; ++i){

		close_thread_and_join(&kernel->thread_list[i]);
	}

	helix_free(kernel->alloc,kernel->thread_list);

	free_stack(kernel->alloc, kernel, HELIX_STACK_SIZE);
}

HELIX_API uintptr_t helix_go(helix_t helix, void(*f)(helix_t, void*), void* userdata){

	helix_kernel_t kernel = helix->runq->helix_kenerl;
}

//////////////////////////////////////////////////////////////////////////

HELIX_PRIVATE void helix_actor_sleep(helix_kernel_t /*kernel*/ ,helix_t /*actor*/){

}