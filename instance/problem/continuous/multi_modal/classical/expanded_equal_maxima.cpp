#include "expanded_equal_maxima.h"

namespace OFEC {

	expanded_equal_maxima::expanded_equal_maxima(param_map &v) :
		expanded_equal_maxima((v.at("proName")), (v.at("numDim")), 1) {

		
	}
	expanded_equal_maxima::expanded_equal_maxima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_equal_maxima::initialize() {
		set_range(-40, 40);
		set_init_range(-40., 40.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_objective_monitor = true;
		// 5^Dim gopt 
		size_t num = (int)pow(5, m_variable_size);
		
		std::vector<std::vector<real>> obj_data(num, std::vector<real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	void expanded_equal_maxima::evaluate_objective(real *x, std::vector<real> &obj) {
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
		
		for (i = 0; i<m_variable_size; ++i)
		{
			x[i] += 0.1;
			if ((x[i] <= 1.0)&(x[i] >= 0.0))
			{
				obj[0] += -pow((sin(5 * OFEC_PI *x[i])), 6.0);
			}
			else
			{
				obj[0] += pow(x[i], 2.0);
			}
		}
		obj[0] += 1.0*m_variable_size;

	}

}