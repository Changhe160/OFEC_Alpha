#include "F14_shifted_rotated_expanded_scaffer_F6.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(param_map &v) :
			F14_shifted_rotated_expanded_scaffer_F6((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			scaffer_F6(name, size_var, size_obj) {
			
		}

		void F14_shifted_rotated_expanded_scaffer_F6::initialize() {
			set_range(-100, 100);

			set_original_global_opt();				
			set_condition_number(3);
			set_bias(-300);			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}
		void F14_shifted_rotated_expanded_scaffer_F6::evaluate_objective(real *x, std::vector<real> &obj) {
			scaffer_F6::evaluate_objective(x, obj);
		}
	}
}