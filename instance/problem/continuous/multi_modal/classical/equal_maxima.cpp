#include "equal_maxima.h"

namespace OFEC {
	
		equal_maxima::equal_maxima(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {

			v[param_numDim] = 1;
			set_range(0, 1.); // note
			initialize();
		}
		equal_maxima::equal_maxima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(0, 1.); // note
			initialize();
		}
		void equal_maxima::initialize() { // note
			m_opt_mode[0] = optimization_mode::Maximization;
			m_objective_accuracy = 0.01;
			m_variable_accuracy = 1.e-4;
			m_original_optima.set_number_variable(5);
			std::vector<std::vector<real>> obj_data(5, std::vector<real>(1, 1.));
			for (auto &i : obj_data) {
				m_original_optima.append(i[0]);
			}
			m_optima = m_original_optima;
			add_tag(problem_tag::MMP);

		}
		void equal_maxima::evaluate__(real *x, std::vector<real>& obj) {
			double s;
			s = pow(sin(5 * OFEC_PI*x[0]), 6);
			obj[0] = s + m_bias;  // note
		}
	
}