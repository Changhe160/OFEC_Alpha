#include "expanded_five_uneven_peak_trap.h"

namespace OFEC {

	expanded_five_uneven_peak_trap::expanded_five_uneven_peak_trap(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-40, 40);
		initialize();
	}
	expanded_five_uneven_peak_trap::expanded_five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-40, 40);
		initialize();
	}

	void expanded_five_uneven_peak_trap::initialize() {
		m_opt_mode[0] = optimization_mode::Minimization;
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		// 2^Dim gopt 
		size_t num = (int)pow(2, m_variable_size);
		m_original_optima.set_number_variable(num);
		std::vector<std::vector<real>> obj_data(num, std::vector<real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
	}
	void expanded_five_uneven_peak_trap::evaluate__(real *x, std::vector<real>& obj) {
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
			if (x[i]<0)
			{
				obj[0] += -200.0 + pow(x[i], 2.0);
			}
			else if (x[i]<2.5)
			{
				obj[0] += -80.0*(2.5 - x[i]);
			}
			else if (x[i]<5.0)
			{
				obj[0] += -64.0*(x[i] - 2.5);
			}
			else if (x[i]<7.5)
			{
				obj[0] += -160.0 + pow(x[i], 2.0);
			}
			else if (x[i]<12.5)
			{
				obj[0] += -28.0*(x[i] - 7.5);
			}
			else if (x[i]<17.5)
			{
				obj[0] += -28.0*(17.5 - x[i]);
			}
			else if (x[i]<22.5)
			{
				obj[0] += -32.0*(x[i] - 17.5);
			}
			else if (x[i]<27.5)
			{
				obj[0] += -32.0*(27.5 - x[i]);
			}
			else if (x[i] <= 30.0)
			{
				obj[0] += -80.0*(x[i] - 27.5);
			}
			else
			{
				obj[0] += -200.0 + pow(x[i] - 30.0, 2.0);
			}
		}
		obj[0] += 200.0*m_variable_size;

	}

}