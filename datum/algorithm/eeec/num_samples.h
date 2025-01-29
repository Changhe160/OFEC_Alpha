#ifndef OFEC_DATUM_NUM_SAMPLES_H
#define OFEC_DATUM_NUM_SAMPLES_H

#include "../../../core/datum_base.h"
#include <cstddef>

namespace ofec {
	struct DatumNumSamples : public DatumBase {
		size_t value;
	};
	extern thread_local DatumNumSamples g_num_samples;
}

#endif // !OFEC_DATUM_NUM_SAMPLES_H
