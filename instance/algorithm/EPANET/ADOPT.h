#ifndef OFEC_ADOPT_H
#define OFEC_ADOPT_H

#include "../../../core/algorithm/multi_population.h"
#include "../../../core/measure/measure.h"
#include "ADOPT_individual.h"
#include "ADOPT_sub_population.h"
#include <cmath>

namespace OFEC {
	class ADOPT : public population<ADOPT_individual>
	{
	public:
		ADOPT(param_map & v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		
		void update_distance(size_t idx, ADOPT_individual & individual);
		bool update_alternative_solution(size_t idx, ADOPT_individual & indi);
	private:
		float m_tar;
		multi_population<ADOPT_sub_population> m_sub_population;
		std::vector<std::pair<std::unique_ptr<ADOPT_individual>,size_t>> m_alternative;
		
	};

}
#endif // OFEC_ADOPT_H

