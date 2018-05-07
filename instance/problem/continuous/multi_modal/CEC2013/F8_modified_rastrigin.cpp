#include "F8_modified_rastrigin.h"

namespace OFEC {
	namespace CEC2013 {

		F8_modified_rastrigin::F8_modified_rastrigin(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {}

		F8_modified_rastrigin::F8_modified_rastrigin(param_map & v) : F8_modified_rastrigin((v.at("proName")), (v.at("numDim")), 1) {}

		void F8_modified_rastrigin::initialize() {
			m_opt_mode[0] = optimization_mode::Maximization;
			m_k.resize(2);
			m_k[0] = 3;
			m_k[1] = 4;
			m_variable_accuracy = 0.01;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_objective_monitor = true;
			size_t num_global_optima = 1;
			for (size_t i = 0; i < 2; i++)
				num_global_optima *= m_k[i];
			for (size_t i = 0; i < num_global_optima; ++i)
				m_original_optima.append(-2.);
			m_optima = m_original_optima;
		}

		void F8_modified_rastrigin::evaluate__(real * x, std::vector<real>& obj) {
			double s = 0;
			for (int i = 0; i < m_variable_size; ++i) {
				s += 10 + 9 * cos(2 * OFEC_PI*m_k[i] * x[i]);
			}
			obj[0] = -s;
		}
	}
}