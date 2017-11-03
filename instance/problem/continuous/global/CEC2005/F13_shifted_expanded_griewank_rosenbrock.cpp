#include "F13_shifted_expanded_griewank_rosenbrock.h"

namespace OFEC {
	namespace CEC2005 {
		F13_shifted_expanded_griewank_rosenbrock::F13_shifted_expanded_griewank_rosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-5, 5);
			initialize();
		}
		F13_shifted_expanded_griewank_rosenbrock::F13_shifted_expanded_griewank_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-5, 5);
			initialize();
		}

		void F13_shifted_expanded_griewank_rosenbrock::initialize() {

			set_original_global_opt();
			set_bias(-130);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path

			set_global_opt(m_translation.data());
		}
		void F13_shifted_expanded_griewank_rosenbrock::evaluate__(real *x, std::vector<real>& obj) {
			if (m_translation_flag) {
				translate(x);
				translate_origin(x);
			}
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
}