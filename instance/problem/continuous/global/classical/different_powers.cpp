#include "different_powers.h"

namespace OFEC {

	different_powers::different_powers(param_map &v) :
		different_powers((v.at("proName")), (v.at("numDim")), 1) {

	}
	different_powers::different_powers(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void different_powers::initialize() {
		m_variable_monitor = true;
		set_range(-100., 100.);
		set_init_range(-100., 100.);

		set_original_global_opt();

		set_global_opt();
		m_initialized = true;
	}
	void different_powers::evaluate_objective(real *x, std::vector<real> &obj) {
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
			obj[0] += pow(fabs(x[i]), 2.0 + 4.0*i / (m_variable_size - 1.0));
		}
		obj[0] = pow(obj[0], 0.5);
	}

}