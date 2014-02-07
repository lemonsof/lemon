#include <helix/utility.h>

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


HELIX_PRIVATE helix_alloc_t* get_default_alloc(){
	return &default_alloc.alloc;
}