#ifndef OFEC_DATUM_SEEDS_H
#define OFEC_DATUM_SEEDS_H

#include "../../../core/datum_base.h"
#include "../../../core/problem/solution.h"
#include <list>

namespace ofec {
	struct DatumSeeds : public DatumBase {
		std::list<const ofec::SolutionBase*> sols;
	};
	extern thread_local DatumSeeds g_seeds;
}

#endif // !OFEC_DATUM_SEEDS_H
