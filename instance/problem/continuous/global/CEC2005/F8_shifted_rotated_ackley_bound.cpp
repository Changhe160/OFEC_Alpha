#include "F8_shifted_rotated_ackley_bound.h"

namespace OFEC {
	namespace CEC2005 {
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			ackley((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			initialize();
		}

		void F8_shifted_rotated_ackley_bound::initialize() {

			set_bias(-140);
			set_condition_number(100);
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
				m_translation_flag = true;
			}
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F8_shifted_rotated_ackley_bound::evaluate__(real *x, vector<real>& obj) {

			ackley::evaluate__(x, obj);
		}
	}
}