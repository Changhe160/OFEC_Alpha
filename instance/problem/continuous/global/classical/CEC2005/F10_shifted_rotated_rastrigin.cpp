#include "F10_shifted_rotated_rastrigin.h"

namespace OFEC {
	F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		rastrigin((v[param_proName]), (v[param_numDim]), 1) {
		initialize();
	}
	F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		rastrigin(name, size_var, size_obj) {
		initialize();
	}

	F10_shifted_rotated_rastrigin::~F10_shifted_rotated_rastrigin() {
		//dtor
	}

	void F10_shifted_rotated_rastrigin::initialize() {

		set_bias(-330);
		set_condition_number(2);

		m_translation.resize(m_variable_size);
		bool is_load = load_translation("instance/problem/continuous/global/classical/CEC2005/data/");  //data path
		if (!is_load) {
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);
			m_translation_flag = true;
		}
		load_rotation("instance/problem/continuous/global/classical/CEC2005/data/");
		clear_global_opt();
		set_global_opt(m_translation.data());
	}
	void F10_shifted_rotated_rastrigin::evaluate__(real *x, vector<real>& obj) {
		
		rastrigin::evaluate__(x, obj);
	}
}