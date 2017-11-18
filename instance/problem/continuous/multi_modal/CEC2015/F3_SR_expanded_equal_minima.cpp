#include "F3_SR_expanded_equal_minima.h"

namespace OFEC {
	namespace CEC2015 {
		F3_SR_expanded_equal_minima::F3_SR_expanded_equal_minima(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			CEC2015_function((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F3_SR_expanded_equal_minima::F3_SR_expanded_equal_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			initialize();
		}

		void F3_SR_expanded_equal_minima::initialize() {
			set_bias(300.);
			set_scale(20.);
			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 5^Dim gopt 
			evaluate_optima();

			add_tag(problem_tag::MMP);
		}
		void F3_SR_expanded_equal_minima::evaluate__(real *x, std::vector<real>& obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

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
			obj[0] += m_bias;
		}
	}
}