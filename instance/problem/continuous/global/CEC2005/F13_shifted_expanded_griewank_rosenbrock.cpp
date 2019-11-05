#include "F13_shifted_expanded_griewank_rosenbrock.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F13_shifted_expanded_griewank_rosenbrock::F13_shifted_expanded_griewank_rosenbrock(param_map &v) :
			F13_shifted_expanded_griewank_rosenbrock((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F13_shifted_expanded_griewank_rosenbrock::F13_shifted_expanded_griewank_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void F13_shifted_expanded_griewank_rosenbrock::initialize() {
			set_range(-3, 1);
			set_original_global_opt();
			set_bias(-130);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path

			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}
		void F13_shifted_expanded_griewank_rosenbrock::evaluate_objective(real *x, std::vector<real> &obj) {
			if (m_translation_flag) {
				translate(x);
				translate_origin(x);
			}
			real result = 0;
			for (size_t i = 0; i < m_variable_size; ++i) {
				real result_f2 = 0;
				real result_f8 = 0;
				real x_front = x[i] + 1;
				real x_back = x[(i + 1) % m_variable_size] + 1;

				result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
				result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((real)(i + 1))) + 1;
				result += result_f8;


			}

			result += m_bias;
			obj[0] = result;
		}
	}
}