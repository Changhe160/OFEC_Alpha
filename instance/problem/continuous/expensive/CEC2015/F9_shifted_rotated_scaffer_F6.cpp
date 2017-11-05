#include "F9_shifted_rotated_scaffer_F6.h"

namespace OFEC {
	namespace CEC2015 {
		F9_shifted_rotated_scaffer_F6::F9_shifted_rotated_scaffer_F6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			scaffer_F6((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F9_shifted_rotated_scaffer_F6::F9_shifted_rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			scaffer_F6(name, size_var, size_obj) {

			initialize();
		}

		void F9_shifted_rotated_scaffer_F6::initialize() {

			set_bias(900);

			
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F9_shifted_rotated_scaffer_F6::evaluate__(real *x, std::vector<real>& obj) {
			scaffer_F6::evaluate__(x, obj);
		}
	}
}