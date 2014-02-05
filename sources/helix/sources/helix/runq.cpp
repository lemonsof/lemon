#include <assert.h>
#include <helix/runq.h>

HELIX_PRIVATE void init_main_thread(helix_kernel_t helix_kenerl, helix_t current, helix_error_code * ){
	current->helix_kenerl = helix_kenerl;
	current->dispatch_thread = NULL;

	current->actor_list_mutex = helix_create_mutex();

	current->actor_list_condition_variable = helix_create_condition();
}

HELIX_PRIVATE void init_thread(helix_kernel_t helix_kenerl, helix_t current, helix_error_code * ){
	current->helix_kenerl = helix_kenerl;
	current->dispatch_thread = helix_create_thread((void(*)(void*))&helix_thread_run, current);
	current->actor_list_mutex = helix_create_mutex();

	current->actor_list_condition_variable = helix_create_condition();
}

HELIX_PRIVATE helix_actor_t helix_actor_pop(helix_t current){
	if (current->actor_list_header == NULL){
		return NULL;
	}

	helix_actor_t result = current->actor_list_header;

	current->actor_list_header  = result->next;
	
	if (current->actor_list_header == NULL){
		assert(current->actor_list_tail == result);
		current->actor_list_tail = NULL;
	}

	return result;
}

HELIX_PRIVATE void helix_thread_run(helix_t current){
	
	helix_mutex_lock(current->actor_list_mutex);

	while (current->helix_status == helix_running){

		helix_actor_t actor = helix_actor_pop(current);

		if (actor == NULL){
			helix_condition_wait(current->actor_list_condition_variable, current->actor_list_mutex);
			continue;
		}


	}
}

HELIX_PRIVATE void close_thread_and_join(helix_t current){
	
	helix_mutex_lock(current->actor_list_mutex);
	
	current->helix_status = helix_exited;

	helix_condition_notify_all(current->actor_list_condition_variable);
}