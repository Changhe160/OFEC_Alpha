#include "F7_shifted_rotated_griewank_no_bound.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F7_shifted_rotated_griewank_no_bound::F7_shifted_rotated_griewank_no_bound(param_map &v) :
			F7_shifted_rotated_griewank_no_bound((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F7_shifted_rotated_griewank_no_bound::F7_shifted_rotated_griewank_no_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			griewank(name, size_var, size_obj) {
			
			
		}

		void F7_shifted_rotated_griewank_no_bound::initialize() {
			set_range(-1000, 1000);
			set_init_range(0, 600.);
			set_bias(-180);
			set_condition_number(3);
			set_original_global_opt();
			
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
		void F7_shifted_rotated_griewank_no_bound::evaluate_objective(real *x, std::vector<real> &obj) {

			griewank::evaluate_objective(x, obj);
		}
	}
}