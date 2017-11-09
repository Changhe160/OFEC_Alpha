#include "different_powers.h"

namespace OFEC {

	different_powers::different_powers(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100, 100);
		initialize();
	}
	different_powers::different_powers(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-100, 100);
		initialize();
	}

	void different_powers::initialize() {
		
		set_original_global_opt();

		set_global_opt();
	}
	void different_powers::evaluate__(real *x, std::vector<real>& obj) {
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