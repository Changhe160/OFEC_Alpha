#include "uneven_de_maxima.h"

namespace OFEC {
	
	uneven_de_maxima::uneven_de_maxima(param_map &v) : 	uneven_de_maxima((v.at("proName"))) {
	
	}

	uneven_de_maxima::uneven_de_maxima(const std::string &name) : problem(name, 1, 1), function(name, 1, 1) {
		
	}

	void uneven_de_maxima::initialize() { 
		set_range(0, 1.); // note
		set_init_range(0, 1.); // note

		m_opt_mode[0] = optimization_mode::Maximization;
		if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
			global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
		m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
		m_variable_accuracy = 0.01;
		m_objective_monitor = true;
		
		std::vector<std::vector<real>> obj_data(1, std::vector<real>(m_objective_size, 1.));
		for (auto &i : obj_data)
			m_original_optima.append(i);
		m_optima = m_original_optima;
		m_initialized = true;
	}

	void uneven_de_maxima::evaluate_objective(real *x, std::vector<real> &obj) {
		real tmp1 = -2 * log(2)*((x[0] - 0.08) / 0.854)*((x[0] - 0.08) / 0.854);
		real tmp2 = sin(5 * OFEC_PI*(pow(x[0], 3.0 / 4.0) - 0.05));
		real s = exp(tmp1) * pow(tmp2, 6);
		obj[0] = s + m_bias;  // note
	}
	
}