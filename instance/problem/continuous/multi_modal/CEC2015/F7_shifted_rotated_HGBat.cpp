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

		F7_shifted_rotated_HGBat::~F7_shifted_rotated_HGBat() {
			//dtor
		}
		void F7_shifted_rotated_HGBat::initialize() {

			set_bias(700);

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
		void F7_shifted_rotated_HGBat::evaluate__(real *x, vector<real>& obj) {
			HGBat::evaluate__(x, obj);
		}
	}
}