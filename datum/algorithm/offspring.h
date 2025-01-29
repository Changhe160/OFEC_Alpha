#ifndef OFEC_DATUM_OFFSPRING_H
#define OFEC_DATUM_OFFSPRING_H

#include <vector>
#include "../../core/problem/solution.h"
#include "../../core/datum_base.h"

namespace ofec {
	struct DatumOffspring : public DatumBase {
		std::vector<const ofec::SolutionBase *> sols;
	};
	extern thread_local DatumOffspring g_offspring;
}

#endif // !OFEC_DATUM_OFFSPRING_H
