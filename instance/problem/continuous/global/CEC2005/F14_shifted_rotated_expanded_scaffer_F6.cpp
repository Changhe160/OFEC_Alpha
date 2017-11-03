#include "F14_shifted_rotated_expanded_scaffer_F6.h"

namespace OFEC {
	namespace CEC2005 {
		F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			scaffer_F6((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			scaffer_F6(name, size_var, size_obj) {
			initialize();
		}

		void F14_shifted_rotated_expanded_scaffer_F6::initialize() {

			set_condition_number(3);
			set_bias(-300);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
		}
		void F14_shifted_rotated_expanded_scaffer_F6::evaluate__(real *x, std::vector<real>& obj) {
			scaffer_F6::evaluate__(x, obj);
		}
	}
}