#include "expanded_decreasing_minima.h"

namespace OFEC {

<<<<<<< HEAD
	expanded_decreasing_minima::expanded_decreasing_minima(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
		function((v.at("proName")), (v.at("numDim")), 1) {
=======
	expanded_decreasing_minima::expanded_decreasing_minima(param_map &v) :
		expanded_decreasing_minima((v.at("proName")), (v.at("numDim")), 1) {
>>>>>>> zhou_li

		
	}
	expanded_decreasing_minima::expanded_decreasing_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_decreasing_minima::initialize() {
		set_range(-40, 40);
		set_init_range(-40., 40.);
<<<<<<< HEAD
		m_opt_mode[0] = optimization_mode::Minimization;
=======
		
>>>>>>> zhou_li
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_variable_monitor = true;
		// 1 gopt 
		std::vector<std::vector<real>> var_data(1, std::vector<real>(m_variable_size, 0));

		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		
	}
	void expanded_decreasing_minima::evaluate__(real *x, std::vector<real>& obj) {
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
				obj[0] += -exp(-2.0*log(2.0)*pow((x[i] - 0.1) / 0.8, 2.0))*pow(sin(5.0*OFEC_PI*x[i]), 6.0);
			}
			else
			{
				obj[0] += pow(x[i], 2.0);
			}
		}
		obj[0] += 1.0*m_variable_size;

	}

}