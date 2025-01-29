/********* Begin Register Information **********
{
	"name": "RW-CEC2011-T02",
	"identifier": "RW_CEC2011_T2",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

#ifndef OFEC_RW_CEC2011_T2_H
#define OFEC_RW_CEC2011_T2_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace cec2011 {
		class LennardJonesPotential : virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(LennardJonesPotential)
		protected:
			virtual void initialize_(Environment* env) override;
			void addInputParameters();
			void updateOptima(Environment* env) override;
			void evaluateObjective(Real* x, std::vector<Real>& obj)const override;
		};
	}
	using RW_CEC2011_T2 = cec2011::LennardJonesPotential;
}

#endif // !OFEC_RW_CEC2011_T2_H
