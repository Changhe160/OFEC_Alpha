#include "F8_shifted_rotated_ackley_bound.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(param_map &v) :
			F8_shifted_rotated_ackley_bound((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			
		}

		void F8_shifted_rotated_ackley_bound::initialize() {
			set_range(-32, 32);

			set_original_global_opt();
			set_bias(-140);
			set_condition_number(100);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			for (size_t j = 1; j < m_variable_size / 2 + 1; ++j) {
				m_translation[(2 * j - 1) - 1] = -32;
				m_translation[2 * j - 1] = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(-32, 32);
			}
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}
		void F8_shifted_rotated_ackley_bound::evaluate_objective(real *x, std::vector<real> &obj) {

			ackley::evaluate_objective(x, obj);
		}
	}
}