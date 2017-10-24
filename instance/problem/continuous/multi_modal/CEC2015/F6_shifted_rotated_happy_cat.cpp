#include "F6_shifted_rotated_happy_cat.h"

namespace OFEC {
	namespace CEC2015 {
		F6_shifted_rotated_happy_cat::F6_shifted_rotated_happy_cat(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			happy_cat((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F6_shifted_rotated_happy_cat::F6_shifted_rotated_happy_cat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			happy_cat(name, size_var, size_obj) {

			initialize();
		}

		F6_shifted_rotated_happy_cat::~F6_shifted_rotated_happy_cat() {
			//dtor
		}
		void F6_shifted_rotated_happy_cat::initialize() {

			set_bias(0);

			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");
			m_optima.clear();
			set_global_opt();
		}
		void F6_shifted_rotated_happy_cat::evaluate__(real *x, vector<real>& obj) {
			happy_cat::evaluate__(x, obj);
		}
	}
}