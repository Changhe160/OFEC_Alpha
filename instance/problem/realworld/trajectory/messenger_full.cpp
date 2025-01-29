#include "messenger_full.h"
#include "gtoptoolbox/trajobjfuns.h"

namespace ofec::trajectory {
	void MessengerFull::initialize_(Environment* env) {
		Continuous::initialize_(env);
		resizeVariable(26);
		m_domain.setRange(1900, 2300, 0);
		m_domain.setRange(2.5, 4.05, 1);
		m_domain.setRange(0, 1, 2);
		m_domain.setRange(0, 1, 3);
		for (size_t i = 4; i < 9; ++i)
			m_domain.setRange(100, 500, i);
		m_domain.setRange(100, 600, 9);
		for (size_t i = 10; i < 16; ++i)
			m_domain.setRange(0.01, 0.99, i);
		m_domain.setRange(1.1, 6, 16);
		m_domain.setRange(1.1, 6, 17);
		for (size_t i = 18; i < 21; ++i)
			m_domain.setRange(1.05, 6, i);
		for (size_t i = 21; i < 26; ++i)
			m_domain.setRange(-OFEC_PI, OFEC_PI, i);
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
	}

	void MessengerFull::evaluateObjective(Real *x, std::vector<Real> &obj)const {
		std::vector<double> vx(x, x + m_number_variables);
		obj[0] = gtop_toolbox::messengerfull(vx);
	}
}