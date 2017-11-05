#include "F2_rotated_discus.h"

namespace OFEC {
	namespace CEC2015 {
		F2_rotated_discus::F2_rotated_discus(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			discus((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F2_rotated_discus::F2_rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			discus(name, size_var, size_obj) {

			initialize();
		}

		void F2_rotated_discus::initialize() {

			set_bias(200);
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt();
		}
		void F2_rotated_discus::evaluate__(real *x, std::vector<real>& obj) {
			discus::evaluate__(x, obj);
		}
	}
}