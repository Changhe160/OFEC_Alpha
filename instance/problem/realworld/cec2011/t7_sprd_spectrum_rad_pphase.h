/********* Begin Register Information **********
{
	"name": "RW-CEC2011-T07",
	"identifier": "RW_CEC2011_T7",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

#ifndef OFEC_RW_CEC2011_T7_H
#define OFEC_RW_CEC2011_T7_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace cec2011 {
		class SprdSpectrumRadPphase :virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(SprdSpectrumRadPphase)
		protected:
			virtual void initialize_(Environment* env) override;
			void addInputParameters();
			void evaluateObjective(Real* x, std::vector<Real>& obj) const override;
		};
	}
	using RW_CEC2011_T7 = cec2011::SprdSpectrumRadPphase;
}

#endif // !OFEC_RW_CEC2011_T7_H
