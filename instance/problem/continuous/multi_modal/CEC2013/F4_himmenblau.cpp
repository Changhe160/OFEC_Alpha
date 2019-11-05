#include "F4_himmenblau.h"

namespace OFEC {
	namespace CEC2013 {
		F4_himmenblau::F4_himmenblau(const std::string & name) : problem(name, 2, 1), function(name, 2, 1) {}

		F4_himmenblau::F4_himmenblau(param_map & v) : F4_himmenblau(v.at("proName")) {}

		void F4_himmenblau::initialize() {
			set_range(-6, 6);
			set_init_range(-6, 6);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.01;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			// 4 gopt
			for (size_t i = 0; i < 4; i++)
				m_original_optima.append(200.0);
			m_optima = m_original_optima;
			m_objective_monitor = true;
			m_variable_partition = { {0,1} };
			m_initialized = true;
		}

		void F4_himmenblau::evaluate_objective(real *x, std::vector<real> &obj) {
			real s = 0;
			real t0 = (x[0] * x[0] + x[1] - 11), t1 = (x[1] * x[1] + x[0] - 7);
			s = 200 - (t0*t0 + t1 * t1);
			obj[0] = s + m_bias;
		}
	}
}