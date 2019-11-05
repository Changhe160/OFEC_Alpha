#include "F10_shifted_rotated_rastrigin.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(param_map &v) :
			F10_shifted_rotated_rastrigin((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {
		
		}

		void F10_shifted_rotated_rastrigin::initialize() {
			set_range(-5, 5);
			set_original_global_opt();
			set_bias(-330);
			set_condition_number(2);

			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
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
		void F10_shifted_rotated_rastrigin::evaluate_objective(real *x, std::vector<real> &obj) {

			rastrigin::evaluate_objective(x, obj);
		}
	}
}