#include "F5_six_hump_camel_back.h"

namespace OFEC {
	namespace CEC2013 {
		F5_six_hump_camel_back::F5_six_hump_camel_back(const std::string & name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {}

		F5_six_hump_camel_back::F5_six_hump_camel_back(param_map & v) : F5_six_hump_camel_back((v.at("proName")), (v.at("numDim")), 1){}

		void F5_six_hump_camel_back::initialize() {
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.5;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_objective_monitor = true;
			// 2 gopt
			for (size_t i = 0; i < 4; i++)
				m_original_optima.append(1.031628453);
			m_optima = m_original_optima;
		}

		void F5_six_hump_camel_back::evaluate__(real * x, std::vector<real>& obj) {
			double s = -((4 - 2.1*x[0] * x[0] + pow(x[0], 4.0) / 3.0)*x[0] * x[0] \
				+ x[0] * x[1] + (4 * x[1] * x[1] - 4)*x[1] * x[1]);
			obj[0] = s + m_bias;
		}
	}
}