#include "F3_shifted_rotated_weierstrass.h"

namespace OFEC {
	namespace CEC2015 {
		F3_shifted_rotated_weierstrass::F3_shifted_rotated_weierstrass(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			weierstrass((v.at("proName")), (v.at("numDim")), 1) {

			initialize();
		}
		F3_shifted_rotated_weierstrass::F3_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			weierstrass(name, size_var, size_obj) {

			initialize();
		}

		void F3_shifted_rotated_weierstrass::initialize() {
			m_variable_monitor = true;
			set_bias(300);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F3_shifted_rotated_weierstrass::evaluate__(real *x, std::vector<real>& obj) {
			weierstrass::evaluate__(x, obj);
		}
	}
}