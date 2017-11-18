#include "griewank_rosenbrock.h"

namespace OFEC {
	
	griewank_rosenbrock::griewank_rosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		set_range(-5, 5);
		set_init_range(-5., 5.);
		initialize();
	}
	griewank_rosenbrock::griewank_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-5, 5);
		set_init_range(-5., 5.);
		initialize();
	}

	void griewank_rosenbrock::initialize() {

		set_original_global_opt();

		set_global_opt();
	}
	void griewank_rosenbrock::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		double result = 0;
		for (size_t i = 0; i < m_variable_size; ++i) {
			double result_f2 = 0;
			double result_f8 = 0;
			double x_front = x[i] + 1;
			double x_back = x[(i + 1) % m_variable_size] + 1;

			result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
			result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((double)(i + 1))) + 1;
			result += result_f8;


		}

		result += m_bias;
		obj[0] = result;
	}
	
}