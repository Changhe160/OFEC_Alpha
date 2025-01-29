#include "f1_shifted_sphere.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedSphere::initialize_(Environment *env) {
			Sphere::initialize_(env);
			setBias(-450);
			loadTranslation("/instance/problem/continuous/single_objective/global/cec2005/GOP_CEC2005_F01");  //data path	
		}

		void ShiftedSphere::updateOptima(Environment *env) {
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-6;
		}
	}
}