#include <helix/abi.h>
#ifdef WIN32
#include <process.h>
typedef struct{
	helix_thread thread;
	HANDLE handle;
} helix_win32_thread;

HELIX_PRIVATE uintptr_t __pid(helix_thread * thread){
	return ::GetThreadId(((helix_win32_thread*)thread)->handle);
}

HELIX_PRIVATE void __join(helix_thread * thread){
	::WaitForSingleObject((((helix_win32_thread*)thread)->handle),INFINITE);
}

HELIX_API helix_thread* helix_create_thread(void(*f)(void*), void* param){

	helix_win32_thread *thread = (helix_win32_thread*)malloc(sizeof(helix_win32_thread));

	thread->handle = (HANDLE)_beginthread(f, NULL, param);

	thread->thread.pid = __pid;

	thread->thread.join = __join;

	return &thread->thread;
}

HELIX_API void helix_release_thread(helix_thread * thread){
	free(thread);
}
#else
# error "not implement"
#endif //WIN32