#ifndef OFEC_DATUM_MULTI_POP_H
#define OFEC_DATUM_MULTI_POP_H

#include <vector>
#include "../../core/problem/solution.h"
#include "../../core/datum_base.h"
#include "../../core/algorithm/population.h"
#include "../../core/algorithm/multi_population.h"

namespace ofec {
	struct DatumMultiPop : public DatumBase {
		std::vector<std::vector<const ofec::SolutionBase*>> pops;
		
		template<typename TPopulation>
		void bindPopulation(const TPopulation& popu) {
			pops.clear();
			pops.resize(1);
			for (size_t i = 0; i < popu.size(); ++i) {
				pops[0].push_back(&popu[i]);
			}
		}

		template<typename TMultiPopulation>
		void bindMultiPopulations(const TMultiPopulation& mulpops) {
			pops.clear();
			pops.resize(mulpops.size());
			for (size_t k = 0; k < mulpops.size(); ++k) {
				for (size_t i = 0; i < mulpops[k].size(); ++i) {
					pops[k].push_back(&mulpops[k][i]);
				}
			}
		}
	};
	extern thread_local DatumMultiPop g_multi_pop;
}

#endif // !OFEC_DATUM_MULTI_POP_H
