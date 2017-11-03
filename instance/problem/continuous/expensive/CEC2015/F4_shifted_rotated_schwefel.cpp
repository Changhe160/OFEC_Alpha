#include "F4_shifted_rotated_schwefel.h"

namespace OFEC {
	namespace CEC2015 {
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			schwefel((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			initialize();
		}

		void F4_shifted_rotated_schwefel::initialize() {

			set_bias(400);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F4_shifted_rotated_schwefel::evaluate__(real *x, std::vector<real>& obj) {
			schwefel::evaluate__(x, obj);
		}
	}
}