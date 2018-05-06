#include "F11_shifted_rotated_weierstrass.h"

namespace OFEC {
	namespace CEC2005 {
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			weierstrass((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			weierstrass(name, size_var, size_obj) {
			
		}

		void F11_shifted_rotated_weierstrass::initialize() {
			m_variable_monitor = true;
			set_range(-0.5, 0.5);
			set_init_range(-0.5, 0.5);

			set_original_global_opt();

			set_condition_number(5);
			set_bias(90);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
		}
		void F11_shifted_rotated_weierstrass::evaluate__(real *x, std::vector<real>& obj) {

			weierstrass::evaluate__(x, obj);
		}
	}
}