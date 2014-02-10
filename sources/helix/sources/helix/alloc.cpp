#include <helix/alloc.hpp>

#ifdef WIN32
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

helix_win_alloc default_alloc_g = { { __alloc } };

#else
# error "not impelment"
#endif 


namespace helix{namespace impl{
	helix_alloc_t* default_alloc()
	{
		return &default_alloc_g.alloc;
	}
}}