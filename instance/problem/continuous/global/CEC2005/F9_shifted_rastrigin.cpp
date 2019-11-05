#include "F9_shifted_rastrigin.h"

namespace OFEC {
	namespace CEC2005 {
		F9_shifted_rastrigin::F9_shifted_rastrigin(param_map &v) :
			F9_shifted_rastrigin((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F9_shifted_rastrigin::F9_shifted_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {
		
		}

		void F9_shifted_rastrigin::initialize() {
			set_range(-5, 5);
			set_original_global_opt();
			set_bias(-330);

			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path

			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.resize(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				m_variable_partition[i] = { i };
			m_initialized = true;
		}
		void F9_shifted_rastrigin::evaluate_objective(real *x, std::vector<real> &obj) {

			rastrigin::evaluate_objective(x, obj);
		}
	}
}