#include "F9_shifted_rastrigin.h"

namespace OFEC {
	namespace CEC2005 {
		F9_shifted_rastrigin::F9_shifted_rastrigin(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			rastrigin((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F9_shifted_rastrigin::F9_shifted_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {
			initialize();
		}

		void F9_shifted_rastrigin::initialize() {

			set_bias(-330);

			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
				m_translation_flag = true;
			}

			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F9_shifted_rastrigin::evaluate__(real *x, vector<real>& obj) {

			rastrigin::evaluate__(x, obj);
		}
	}
}