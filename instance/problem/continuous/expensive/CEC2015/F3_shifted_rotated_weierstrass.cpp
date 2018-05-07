#include "F3_shifted_rotated_weierstrass.h"

namespace OFEC {
	namespace CEC2015 {
		F3_shifted_rotated_weierstrass::F3_shifted_rotated_weierstrass(param_map &v) :F3_shifted_rotated_weierstrass((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F3_shifted_rotated_weierstrass::F3_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			weierstrass(name, size_var, size_obj) {

		
		}

		void F3_shifted_rotated_weierstrass::initialize() {
			set_range(-0.5, 0.5);
			set_init_range(-0.5, 0.5);
			m_variable_monitor = true;
			set_bias(300);
			set_original_global_opt();
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt(m_translation.data());
		}
		void F3_shifted_rotated_weierstrass::evaluate__(real *x, std::vector<real>& obj) {
			weierstrass::evaluate__(x, obj);
		}
	}
}