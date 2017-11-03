#include "F11_shifted_rotated_weierstrass.h"

namespace OFEC {
	namespace CEC2005 {
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			weierstrass((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			weierstrass(name, size_var, size_obj) {
			initialize();
		}

		void F11_shifted_rotated_weierstrass::initialize() {


			set_condition_number(5);
			set_bias(90);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F11_shifted_rotated_weierstrass::evaluate__(real *x, std::vector<real>& obj) {

			weierstrass::evaluate__(x, obj);
		}
	}
}