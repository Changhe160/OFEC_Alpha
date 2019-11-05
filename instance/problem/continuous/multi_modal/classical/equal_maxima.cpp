#include "equal_maxima.h"

namespace OFEC {
	
		equal_maxima::equal_maxima(param_map &v) : equal_maxima((v.at("proName"))) {
	
		}

		equal_maxima::equal_maxima(const std::string &name) :problem(name, 1, 1), function(name, 1, 1) {
			
		}

		void equal_maxima::initialize() {
			set_range(0, 1.); 
			set_init_range(0, 1.);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_objective_monitor = true;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_variable_accuracy = 1.e-2;
			
			std::vector<std::vector<real>> obj_data(5, std::vector<real>(1, 1.));
			for (auto &i : obj_data) {
				m_original_optima.append(i[0]);
			}
			m_optima = m_original_optima;
			m_initialized = true;
		}

		void equal_maxima::evaluate_objective(real *x, std::vector<real> &obj) {
			real s;
			s = pow(sin(5 * OFEC_PI*x[0]), 6);
			obj[0] = s + m_bias;  // note
		}
	
}