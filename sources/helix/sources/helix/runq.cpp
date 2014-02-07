#include <assert.h>
#include <helix/runq.h>
#include <helix/kernel.h>

HELIX_PRIVATE void init_main_runq(helix_kernel_t helix_kenerl, helix_runq_t current){
	memset(current,0,HELIX_HANDLE_SIZEOF(helix_runq_t));
	current->helix_kenerl = helix_kenerl;
	current->dispatch_thread = NULL;

	current->actor_list_mutex = helix_create_mutex();

	current->actor_list_condition_variable = helix_create_condition();

	void * block = (helix_byte_t*)helix_kenerl;

	helix_context_t * context = helix_make_context(block, HELIX_STACK_SIZE, (void(*)(intptr_t))&helix_main_dispatch);

	memcpy(&current->context,context,sizeof(helix_context_t));

	helix_kenerl->main_actor.runq = current;

	helix_kenerl->main_actor.id = 0;

	helix_context_jump(&helix_kenerl->main_actor.context.obj, &current->context, (intptr_t)current);
}

HELIX_PRIVATE void init_runq(helix_kernel_t helix_kenerl, helix_runq_t current){

	memset(current, 0, HELIX_HANDLE_SIZEOF(helix_runq_t));
	current->helix_kenerl = helix_kenerl;
	current->helix_status = helix_running;
	current->dispatch_thread = helix_create_thread((void(*)(void*))&helix_dispatch, current);
	current->actor_list_mutex = helix_create_mutex();

	current->actor_list_condition_variable = helix_create_condition();
}

HELIX_PRIVATE helix_t helix_actor_pop(helix_runq_t current){

	if (current->actor_list_header == NULL){
		return NULL;
	}

	helix_t result = current->actor_list_header;

	current->actor_list_header  = result->next;
	
	if (current->actor_list_header == NULL){
		assert(current->actor_list_tail == result);
		current->actor_list_tail = NULL;
	}
	
	result->next = NULL;

	return result;
}

HELIX_PRIVATE void helix_actor_push(helix_runq_t current, helix_t actor){
	actor->next = NULL;
	if (current->actor_list_tail != NULL){
		assert(current->actor_list_header == NULL);
		current->actor_list_header = current->actor_list_tail = actor;
	} else {
		current->actor_list_tail->next = actor;
		current->actor_list_tail = actor;
	}
}

HELIX_PRIVATE void helix_actor_push_p(helix_runq_t current, helix_t actor){
	helix_mutex_lock(current->actor_list_mutex);
	helix_actor_push(current, actor);
	helix_mutex_unlock(current->actor_list_mutex);
}

HELIX_PRIVATE void helix_main_dispatch(helix_runq_t current){
	
	helix_context_jump(&current->context,&current->helix_kenerl->main_actor.context.obj ,0);

	helix_dispatch(current);
}

HELIX_PRIVATE void helix_dispatch(helix_runq_t current){
	
	helix_mutex_lock(current->actor_list_mutex);

	while (current->helix_status == helix_running){

		helix_t actor = helix_actor_pop(current);

		if (actor == NULL){
			helix_condition_wait(current->actor_list_condition_variable, current->actor_list_mutex);
			continue;
		}

		helix_mutex_unlock(current->actor_list_mutex);
		//run current actor
		helix_context_jump(&current->context, actor->context.ptr, (intptr_t)current);
		//push the actor to sleep queue
		helix_actor_sleep(current->helix_kenerl,actor);

		helix_mutex_lock(current->actor_list_mutex);
	}

	helix_mutex_unlock(current->actor_list_mutex);
}

HELIX_PRIVATE void close_thread_and_join(helix_runq_t current){
	
	helix_mutex_lock(current->actor_list_mutex);
	
	current->helix_status = helix_exited;

	helix_condition_notify_all(current->actor_list_condition_variable);

	helix_mutex_unlock(current->actor_list_mutex);

	current->dispatch_thread->join(current->dispatch_thread);
}