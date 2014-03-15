#include <helix/abi.h>
#include <helix++/unittest.hpp>

namespace helix{ namespace {
	class helix_actor_unittest{};

	void call(helix_t /*H*/,void * /*userdata*/)
	{
		std::cout << "hell world" << std::endl;
	}


	HELIX_UNITTEST_CASE(helix_actor_unittest,create_test)
	{
	
		helix_declare_errcode(errorCode);

		helix_t H = helix_open(nullptr, &errorCode);

		helix_duration duration = { helix_milliseconds, 100 };

		for (;;)
		{
			std::cout << "hell world" << std::endl;

			helix_check(nullptr == helix_event_poll(H, &duration));

			helix_check(helix_lasterror(H) == nullptr);
		}
	}
} }