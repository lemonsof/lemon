#include <helix/abi.h>
#include <helix++/unittest.hpp>

namespace helix{ namespace {
	class helix_actor_unittest{};

	void call(helix_t /*H*/,void * /*userdata*/){

	}

	HELIX_UNITTEST_CASE(helix_actor_unittest,create_test){
		helix_declare_errcode(errorCode);

		helix_t H = helix_open(NULL,&errorCode);

		helix_check(helix_success(errorCode));

		helix_go(H, &call, NULL);

		helix_close(H);
	}
} }