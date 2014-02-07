#include <helix/stack.h>

#ifdef WIN32

#elif defined(LINUX) || defined(SOLARIS) || defined(AIX)  
HELIX_PRIVATE int hardware_concurrency(){

	return get_nprocs();
}
#else
#error "unsupported os"
#endif 

HELIX_PRIVATE void* alloc_stack(helix_alloc_t * alloc, size_t stacksize, size_t extsize){
	void * block = helix_alloc(alloc, stacksize + extsize);

	memset(block, 0, stacksize + extsize);

	return (helix_byte_t*)block + stacksize;
}

HELIX_PRIVATE void free_stack(helix_alloc_t * alloc, void* stack, size_t stacksize){
	void * block = (helix_byte_t*)stack - stacksize;
	helix_free(alloc, block);
}
