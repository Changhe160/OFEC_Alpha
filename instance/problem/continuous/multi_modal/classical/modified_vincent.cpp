#include "modified_vincent.h"

namespace OFEC {

	modified_vincent::modified_vincent(param_map &v) :
		modified_vincent((v.at("proName")), (v.at("numDim")), 1) {

		
	}
	modified_vincent::modified_vincent(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void modified_vincent::initialize() {
		set_range(-10, 10);
		set_init_range(-10, 10);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_objective_monitor = true;
		// 6^Dim gopt 
		size_t num = (int)pow(6, m_variable_size);
		
		std::vector<std::vector<real>> obj_data(num, std::vector<real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	void modified_vincent::evaluate_objective(real *x, std::vector<real> &obj) {
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
		
		for (i = 0; i<m_variable_size; i++)
		{
			x[i] += 4.1112;
			if ((x[i] >= 0.25)&(x[i] <= 10.0))
			{
				obj[0] += -sin(10.0*log(x[i]));
			}
			else if (x[i]<0.25)
			{
				obj[0] += pow(0.25 - x[i], 2.0) - sin(10.0*log(2.5));
			}
			else
			{
				obj[0] += pow(x[i] - 10, 2.0) - sin(10.0*log(10.0));
			}
		}
		obj[0] /= m_variable_size;
		obj[0] += 1.0;
	}

}