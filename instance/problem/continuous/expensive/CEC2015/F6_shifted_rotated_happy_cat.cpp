#include "F6_shifted_rotated_happy_cat.h"

namespace OFEC {
	namespace CEC2015 {
		F6_shifted_rotated_happy_cat::F6_shifted_rotated_happy_cat(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			happy_cat((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F6_shifted_rotated_happy_cat::F6_shifted_rotated_happy_cat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			happy_cat(name, size_var, size_obj) {

			initialize();
		}

		void F6_shifted_rotated_happy_cat::initialize() {

			set_bias(600);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F6_shifted_rotated_happy_cat::evaluate__(real *x, std::vector<real>& obj) {
			happy_cat::evaluate__(x, obj);
		}
	}
}