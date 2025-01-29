/********* Begin Register Information **********
[
	{ "name":"RW-CEC2011-T05", "identifier":"RW_CEC2011_T5", "tags":[ "continuous", "single-objective" ] },
	{ "name":"RW-CEC2011-T06", "identifier":"RW_CEC2011_T6", "tags":[ "continuous", "single-objective" ] }
]
*********** End Register Information **********/

#ifndef OFEC_RW_CEC2011_T5_T6_H
#define OFEC_RW_CEC2011_T5_T6_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace cec2011 {
		class TersoffPotential : virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(TersoffPotential)
		protected:
			virtual void initialize_(Environment* env) override;
			void addInputParameters();
			void evaluateObjective(Real *x, std::vector<Real> &obj) const override;

			void setParameterT5();
			void setParameterT6();


		protected:
			int m_fun_id = 0;
		private:
			Real m_R1, m_R2, m_A, m_B, m_lamda1, m_lamda2, m_lamda3, m_c, m_d, m_n1, m_gamma, m_h;
			std::vector<double> E;
		};
	}
	class RW_CEC2011_T5 : public cec2011::TersoffPotential { OFEC_CONCRETE_INSTANCE(RW_CEC2011_T5) protected: void addInputParameters() { m_fun_id = 1; } };
	class RW_CEC2011_T6 : public cec2011::TersoffPotential { OFEC_CONCRETE_INSTANCE(RW_CEC2011_T6) protected: void addInputParameters() { m_fun_id = 2; } };

}

#endif // !OFEC_RW_CEC2011_T5_H
