#ifndef OFEC_JDE_H
#define OFEC_JDE_H

#include "../DEindividual.h"
#include "../DEpopulation.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	class jDE :public DEpopulation<DEindividual<>>
	{
	public:
		jDE(param_map &v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		void update_FandCR();
	protected:
		double m_t1, m_t2, m_Fl, m_Fu;
		std::vector<double> mv_F, mv_CR;
	};
}


#endif // OFEC_JDE_H
