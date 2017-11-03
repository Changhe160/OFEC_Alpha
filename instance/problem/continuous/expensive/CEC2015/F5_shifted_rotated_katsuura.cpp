#include "F5_shifted_rotated_katsuura.h"

namespace OFEC {
	namespace CEC2015 {
		F5_shifted_rotated_katsuura::F5_shifted_rotated_katsuura(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			katsuura((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F5_shifted_rotated_katsuura::F5_shifted_rotated_katsuura(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			katsuura(name, size_var, size_obj) {

			initialize();
		}

		void F5_shifted_rotated_katsuura::initialize() {

			set_bias(500);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F5_shifted_rotated_katsuura::evaluate__(real *x, std::vector<real>& obj) {
			katsuura::evaluate__(x, obj);
		}
	}
}