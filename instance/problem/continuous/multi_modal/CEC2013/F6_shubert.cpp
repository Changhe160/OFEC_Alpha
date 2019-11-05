#include  "F6_shubert.h"

namespace OFEC {
	namespace CEC2013 {

		F6_shubert::F6_shubert(const std::string & name, size_t size_var) : problem(name, size_var, 1), function(name, size_var, 1) {}

		F6_shubert::F6_shubert(param_map &v) : F6_shubert((v.at("proName")), (v.at("numDim"))) {}

		void F6_shubert::initialize() {
			if (m_variable_size != 2 && m_variable_size != 3)
				throw myexcept("numDim of F6_shuubert should be 2 or 3");
			set_range(-10, 10);
			set_init_range(-10, 10);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.5;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end()) 
					global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_objective_monitor = true;
			size_t num_global_optima = m_variable_size * (size_t)pow(3, m_variable_size);
			if (m_variable_size == 2) {
				for (size_t i = 0; i < num_global_optima; ++i) {
					m_original_optima.append(186.7309088);
				}
			}
			else {
				for (size_t i = 0; i < num_global_optima; ++i) {
					m_original_optima.append(2709.093505);
				}
			}
			m_optima = m_original_optima;
			m_variable_partition.resize(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				m_variable_partition[i] = { i };
			m_initialized = true;
		}

		void F6_shubert::evaluate_objective(real *x, std::vector<real> &obj) {
			real s = 1;
			for (int i = 0; i < m_variable_size; ++i) {
				real a = 0;
				for (int j = 1; j <= 5; ++j) {
					a += j * cos((j + 1)*x[i] + j);
				}
				s *= a;
			}
			obj[0] = -s;
		}
	}
}