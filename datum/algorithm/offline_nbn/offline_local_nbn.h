#ifndef OFEC_DATUM_OFFLINE_LOCAL_NBN_H
#define OFEC_DATUM_OFFLINE_LOCAL_NBN_H

#include <vector>
#include "../../../core/problem/solution.h"

namespace ofec::datum {
	struct AlgorithmLocalNBN {

		bool m_working = false;
		std::vector<Real> m_fitness;
		std::vector<int> m_belong;
		std::vector<Real> m_dis2parent;
		std::vector<std::shared_ptr<SolutionBase>> m_sols;

		

		
	};
	extern AlgorithmLocalNBN m_algorithm_nbn;
}

#endif // !OFEC_DATUM_OFFLINE_LOCAL_NBN_H
