#include <helix/abi.h>

#ifdef WIN32

typedef struct helix_win32_mutex{
	helix_mutex mutex;
	CRITICAL_SECTION handle;
} helix_win32_mutex;

typedef struct helix_win32_condition{
	helix_condition condition;
	CONDITION_VARIABLE handle;
} helix_win32_condition;

HELIX_PRIVATE void __lock(helix_win32_mutex * mutex){
	EnterCriticalSection(&mutex->handle);
}

HELIX_PRIVATE void __unlock(helix_win32_mutex * mutex){
	LeaveCriticalSection(&mutex->handle);
}

HELIX_PRIVATE bool __trylock(helix_win32_mutex * mutex){
	return TryEnterCriticalSection(&mutex->handle) != FALSE;
}

HELIX_PRIVATE void __notify_one(helix_condition *condition){
	WakeConditionVariable(&((helix_win32_condition*)condition)->handle);
}

HELIX_PRIVATE void __notify_all(helix_condition *condition){
	WakeAllConditionVariable(&((helix_win32_condition*)condition)->handle);
}

HELIX_PRIVATE void __wait(helix_condition *condition,helix_mutex * mutex){
	SleepConditionVariableCS(&((helix_win32_condition*)condition)->handle,&((helix_win32_mutex*)mutex)->handle,INFINITE);
}

HELIX_API helix_mutex* helix_create_mutex(){
	helix_win32_mutex* mutex = (helix_win32_mutex*)malloc(sizeof(helix_win32_mutex));

	InitializeCriticalSection(&mutex->handle);
	
	mutex->mutex.lock = (void(*)(helix_mutex*))&__lock;

	mutex->mutex.unlock = (void(*)(helix_mutex*))&__unlock;

	mutex->mutex.try_lock = (bool(*)(helix_mutex*))&__trylock;

	return &mutex->mutex;
}

HELIX_API void helix_release_mutex(helix_mutex * mutex){
	DeleteCriticalSection(&((helix_win32_mutex*)mutex)->handle);
	free(mutex);
}

HELIX_API helix_condition* helix_create_condition(){
	helix_win32_condition* condition = (helix_win32_condition*)malloc(sizeof(helix_win32_condition));

	InitializeConditionVariable(&condition->handle);

	condition->condition.wait = __wait;

	condition->condition.notify_all = __notify_all;

	condition->condition.notify_one = __notify_one;

	return &condition->condition;
}

HELIX_API void helix_release_condition(helix_condition * condition){
	
	free(condition);
}

#else

#endif 