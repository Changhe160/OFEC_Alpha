#include "sound_waves.h"

namespace ofec {
	void SoundWaves::addInputParameters() {
	}

	void SoundWaves::initialize_(Environment* env) {
		m_number_objectives = 1;
		m_number_variables = 6;
		Continuous::initialize_(env);
		m_optimize_mode.resize(m_number_objectives);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
		m_domain.resize(m_number_variables);
		setDomain(-6.4, 6.35);
	}

	void SoundWaves::updateOptima(Environment* env) {
		Continuous::updateOptima(env);
		//m_optima.reset(new Optima<>());
		Solution<> temp_sol(m_number_objectives, m_number_constraints, m_number_variables);
		temp_sol.variable().vector() = { 1.0,5.0,-1.5,4.8,2.0,4.9 };
		//evaluate_(temp_sol);
		evaluateObjective(temp_sol.variable().data(), temp_sol.objective());
		dynamic_cast<Optima<>&>(*m_optima).appendSolution(temp_sol);
	}

	void SoundWaves::evaluateObjective(Real *x, std::vector<Real> &obj) const {
		Real theta = 2 * OFEC_PI / 100.;
		Real s = 0;
		double y_t, y_0_t;
		for (int t = 0; t < 100; t++) {
			y_t = x[0] * sin(x[1] * t * theta + x[2] * sin(x[3] * t * theta + x[4] * sin(x[5] * t * theta)));
			y_0_t = 1.0 * sin(5.0 * t * theta - 1.5 * sin(4.8 * t * theta + 2.0 * sin(4.9 * t * theta)));
			s += (y_t - y_0_t) * (y_t - y_0_t);
		}
		obj[0] = s;
	}
}