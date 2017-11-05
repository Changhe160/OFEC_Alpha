#include "F7_shifted_rotated_HGBat.h"

namespace OFEC {
	namespace CEC2015 {
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			HGBat((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			HGBat(name, size_var, size_obj) {

			initialize();
		}

		void F7_shifted_rotated_HGBat::initialize() {

			set_bias(700);

			
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F7_shifted_rotated_HGBat::evaluate__(real *x, std::vector<real>& obj) {
			HGBat::evaluate__(x, obj);
		}
	}
}