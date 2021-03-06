#include "expanded_six_hump_camel_back.h"

namespace OFEC {

	expanded_six_hump_camel_back::expanded_six_hump_camel_back(param_map &v) :
		expanded_six_hump_camel_back((v.at("proName")), (v.at("numDim")), 1) {

		
	}
	expanded_six_hump_camel_back::expanded_six_hump_camel_back(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_six_hump_camel_back::initialize() {
		set_range(-40., 40.);
		set_init_range(-40., 40.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_objective_monitor = true;
		// 2^(Dim/2) gopt 
		size_t num = (int)pow(2, m_variable_size / 2);
		
		std::vector<std::vector<real>> obj_data(num, std::vector<real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	void expanded_six_hump_camel_back::evaluate_objective(real *x, std::vector<real> &obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);
		size_t i;
		obj[0] = 0.0;
	
		for (i = 0; i<m_variable_size - 1; i += 2)
		{
			x[i] += 0.089842;
			x[i + 1] -= 0.712656;
			obj[0] += ((4.0 - 2.1*pow(x[i], 2.0) + pow(x[i], 4.0) / 3.0)*pow(x[i], 2.0) + x[i] * x[i + 1] + ((-4.0 + 4.0*pow(x[i + 1], 2.0))*pow(x[i + 1], 2.0)))*4.0;
		}
		obj[0] += 4.126514*m_variable_size / 2.0;
	}

}