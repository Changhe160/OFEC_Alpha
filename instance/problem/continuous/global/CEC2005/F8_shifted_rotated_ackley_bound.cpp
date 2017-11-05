#include "F8_shifted_rotated_ackley_bound.h"

namespace OFEC {
	namespace CEC2005 {
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			ackley((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			initialize();
		}

		void F8_shifted_rotated_ackley_bound::initialize() {

			set_bias(-140);
			set_condition_number(100);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F8_shifted_rotated_ackley_bound::evaluate__(real *x, std::vector<real>& obj) {

			ackley::evaluate__(x, obj);
		}
	}
}