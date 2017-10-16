#include "F1_shifted_sphere.h"

namespace OFEC {
	F1_shifted_sphere::F1_shifted_sphere(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		sphere((v[param_proName]), (v[param_numDim]), 1) {

		initialize();
	}
	F1_shifted_sphere::F1_shifted_sphere(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		sphere(name, size_var, size_obj) {

		initialize();
	}

	F1_shifted_sphere::~F1_shifted_sphere() {
		//dtor
	}
	void F1_shifted_sphere::initialize() {
		
		set_bias(-450);
		m_translation.resize(m_variable_size);
		bool is_load = load_translation("instance/problem/continuous/global/classical/CEC2005/data/");  //data path
		if (!is_load) {
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);
			m_translation_flag = true;
		}
		clear_global_opt();
		set_global_opt(m_translation.data());
	}
	void F1_shifted_sphere::evaluate__(real *x, vector<real>& obj) {
		sphere::evaluate__(x, obj);
	}
}