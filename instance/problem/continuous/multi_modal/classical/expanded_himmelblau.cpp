#include "expanded_himmelblau.h"

namespace OFEC {

<<<<<<< HEAD
	expanded_himmelblau::expanded_himmelblau(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
		function((v.at("proName")), (v.at("numDim")), 1) {
=======
	expanded_himmelblau::expanded_himmelblau(param_map &v) :
		expanded_himmelblau((v.at("proName")), (v.at("numDim")), 1) {
>>>>>>> zhou_li

		
	}
	expanded_himmelblau::expanded_himmelblau(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_himmelblau::initialize() {
		set_range(-40., 10.);
		set_init_range(-40., 10.);
<<<<<<< HEAD
		m_opt_mode[0] = optimization_mode::Minimization;
=======
		
>>>>>>> zhou_li
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_objective_monitor = true;
		// 4^(Dim/2) gopt 
		size_t num = (int)pow(4, m_variable_size/2);
		
		std::vector<std::vector<real>> obj_data(num, std::vector<real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
	}
	void expanded_himmelblau::evaluate__(real *x, std::vector<real>& obj) {
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
			x[i] += 3.0;
			x[i + 1] += 2.0;
			obj[0] += pow((x[i] * x[i] + x[i + 1] - 11.0), 2.0) + pow((x[i] + x[i + 1] * x[i + 1] - 7.0), 2.0);
		}

	}

}