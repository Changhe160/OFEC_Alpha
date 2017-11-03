#include "F7_shifted_rotated_griewank_no_bound.h"

namespace OFEC {
	namespace CEC2005 {
		F7_shifted_rotated_griewank_no_bound::F7_shifted_rotated_griewank_no_bound(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			griewank((v[param_proName]), (v[param_numDim]), 1) {
			set_range(0, 600);
			initialize();
		}
		F7_shifted_rotated_griewank_no_bound::F7_shifted_rotated_griewank_no_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			griewank(name, size_var, size_obj) {
			set_range(0, 600);
			initialize();
		}

		void F7_shifted_rotated_griewank_no_bound::initialize() {

			set_bias(-180);
			set_condition_number(3);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F7_shifted_rotated_griewank_no_bound::evaluate__(real *x, std::vector<real>& obj) {

			griewank::evaluate__(x, obj);
		}
	}
}