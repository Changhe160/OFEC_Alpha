/********* Begin Register Information **********
{
	"name": "RW-CEC2011-T10",
	"identifier": "RW_CEC2011_T10",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/


#ifndef OFEC_RW_CEC2011_T10_H
#define OFEC_RW_CEC2011_T10_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace cec2011 {
		class CircularAntennaArrayDesign : virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(CircularAntennaArrayDesign)
		protected:
			void addInputParameters() {}
			virtual void initialize_(Environment* env) override;
			void evaluateObjective(Real *x, std::vector<Real> &obj) const override;
		private:
			std::vector<Real> m_null;
			Real m_phi_desired;
			Real m_distance;
		public:
			Real trapezoidalcir(const std::vector<double> &x2, double upper, double lower, int N1) const;
			Real array_factorcir(const std::vector<double> &x1, double phi) const;
		};
	}
	using RW_CEC2011_T10 = cec2011::CircularAntennaArrayDesign;
}

#endif // !OFEC_RW_CEC2011_T10_H
