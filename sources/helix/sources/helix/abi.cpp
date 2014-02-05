#include <helix/abi.h>

helix_uuid_t LEMON_WIN32_ERROR_CATALOG = { 0x3eb0937e, 0xdbe9, 0x4946, { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf7 } };

helix_uuid_t LEMON_COM_ERROR_CATALOG = { 0x3eb0937e, 0xdbe9, 0x4946, { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf2 } };

helix_uuid_t LEMON_POSIX_ERROR_CATALOG = { 0x11a7987e, 0xa950, 0x434c, { 0xa4, 0xbb, 0x76, 0xdf, 0x4d, 0x42, 0xa2, 0xe1 } };

helix_uuid_t LEMON_UNITTEST_ERROR_CATALOG = { 0x11a7987e, 0xa950, 0x434c, { 0xa4, 0xbb, 0x76, 0xd1, 0x4d, 0x42, 0xa2, 0xe2 } };


helix_ratio __duration_ratios[] = {
	{ 3600, 1 }, { 60, 1 }, { 1, 1000 }, { 1, 1000000 }, { 1, 1000000000 }
};

HELIX_API void helix_duration_cast(helix_duration * source, helix_duration* target){

	helix_ratio lhs = __duration_ratios[source->duration_t];

	helix_ratio rhs = __duration_ratios[target->duration_t];

	target->count = source->count * lhs.D * rhs.N / lhs.N / rhs.D;
}
