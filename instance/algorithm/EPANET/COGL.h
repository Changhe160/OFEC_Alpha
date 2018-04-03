#ifndef OFEC_COGL_H
#define OFEC_COGL_H

#include "../../../core/algorithm/individual.h"
#include "../../../core/algorithm/population.h"
#include "../../../core/algorithm/multi_population.h"
//#include "../DE/DEindividual.h"
//#include "../DE/DEpopulation.h"
#include "../../problem/realworld/EPANET/epa_encoding.h"
#include "../../problem/realworld/EPANET/epanet.h"
#include "../../../core/measure/measure.h"

namespace OFEC {
	class COGL : public population<individual<variable_epanet, real>>, public multi_population<population<individual<variable_epanet, real>>>
	{
	public:
		COGL(param_map & v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		bool is_feasible_population(const population<individual<variable_epanet, real>> & subpopulation);
	private:
		size_t m_num_pop;
		float m_tar;
	};

}
#endif // OFEC_COGL_H

