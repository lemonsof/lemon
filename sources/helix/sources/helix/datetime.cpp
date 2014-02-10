#include <helix/datetime.h>

static helix_ratio __duration_ratios[] = {
	{ 3600, 1 }, { 60, 1 }, { 1, 1000 }, { 1, 1000000 }, { 1, 1000000000 }
};

HELIX_API void helix_duration_cast(helix_duration * source, helix_duration* target){

	helix_ratio lhs = __duration_ratios[source->duration_t];

	helix_ratio rhs = __duration_ratios[target->duration_t];

	target->count = source->count * lhs.D * rhs.N / lhs.N / rhs.D;
}