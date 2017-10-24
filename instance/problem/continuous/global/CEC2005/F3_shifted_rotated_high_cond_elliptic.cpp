#include "F3_shifted_rotated_high_cond_elliptic.h"

namespace OFEC {
	namespace CEC2005 {
		F3_shifted_rotated_high_cond_elliptic::F3_shifted_rotated_high_cond_elliptic(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			elliptic((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F3_shifted_rotated_high_cond_elliptic::F3_shifted_rotated_high_cond_elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			elliptic(name, size_var, size_obj) {

			initialize();
		}

		F3_shifted_rotated_high_cond_elliptic::~F3_shifted_rotated_high_cond_elliptic() {
			//dtor
		}
		void F3_shifted_rotated_high_cond_elliptic::initialize() {

			set_bias(-450);
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
		void F3_shifted_rotated_high_cond_elliptic::evaluate__(real *x, vector<real>& obj) {

			elliptic::evaluate__(x, obj);
		}
	}
}