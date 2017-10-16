#include "F14_shifted_rotated_expanded_scaffer_F6.h"

namespace OFEC {
	F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		scaffer_F6((v[param_proName]), (v[param_numDim]), 1) {
		initialize();
	}
	F14_shifted_rotated_expanded_scaffer_F6::F14_shifted_rotated_expanded_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		scaffer_F6(name, size_var, size_obj) {
		initialize();
	}

	F14_shifted_rotated_expanded_scaffer_F6::~F14_shifted_rotated_expanded_scaffer_F6() {
		//dtor
	}

	void F14_shifted_rotated_expanded_scaffer_F6::initialize() {
		
		set_condition_number(3);
		set_bias(-300);
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
		m_variable_accuracy = 1.0e-2;
	}
	void F14_shifted_rotated_expanded_scaffer_F6::evaluate__(real *x, vector<real>& obj) {
		scaffer_F6::evaluate__(x, obj);
	}
}