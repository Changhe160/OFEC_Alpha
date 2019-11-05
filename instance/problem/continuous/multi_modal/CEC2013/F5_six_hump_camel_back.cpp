#include "F5_six_hump_camel_back.h"

namespace OFEC {
	namespace CEC2013 {
		F5_six_hump_camel_back::F5_six_hump_camel_back(const std::string & name) :problem(name, 2, 1), function(name, 2, 1) {}

		F5_six_hump_camel_back::F5_six_hump_camel_back(param_map & v) : F5_six_hump_camel_back(v.at("proName")) {}

		void F5_six_hump_camel_back::initialize() {
			std::vector<std::pair<real, real>> ranges({ {-1.9,1.9},{-1.1,1.1} });
			set_range(ranges);
			set_init_range(ranges);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.5;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_objective_monitor = true;
			// 2 gopt
			for (size_t i = 0; i < 2; i++)
				m_original_optima.append(1.031628453);
			m_optima = m_original_optima;
			m_variable_partition = { {0,1} };
			m_initialized = true;
		}

		void F5_six_hump_camel_back::evaluate_objective(real *x, std::vector<real> &obj) {
			real s = -((4 - 2.1*x[0] * x[0] + pow(x[0], 4.0) / 3.0)*x[0] * x[0] \
				+ x[0] * x[1] + (4 * x[1] * x[1] - 4)*x[1] * x[1]);
			obj[0] = s + m_bias;
		}
	}
}