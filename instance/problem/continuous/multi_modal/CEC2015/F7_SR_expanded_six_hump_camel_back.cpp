#include "F7_SR_expanded_six_hump_camel_back.h"

namespace OFEC {
	namespace CEC2015 {
		F7_SR_expanded_six_hump_camel_back::F7_SR_expanded_six_hump_camel_back(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			CEC2015_function((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F7_SR_expanded_six_hump_camel_back::F7_SR_expanded_six_hump_camel_back(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			initialize();
		}

		void F7_SR_expanded_six_hump_camel_back::initialize() {
			set_bias(700.);
			set_scale(20.);
			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 2^(Dim/2) gopt

			evaluate_optima();

			add_tag(problem_tag::MMP);
		}
		void F7_SR_expanded_six_hump_camel_back::evaluate__(real *x, std::vector<real>& obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_variable_size - 1; i = i + 2)
			{
				x[i] += 0.089842;
				x[i + 1] += -0.712656;//shift to orgin
				obj[0] += ((4.0 - 2.1*pow(x[i], 2.0) + pow(x[i], 4.0) / 3.0)*pow(x[i], 2.0) + x[i] * x[i + 1] + ((-4.0 + 4.0*pow(x[i + 1], 2.0))*pow(x[i + 1], 2.0)))*4.0;
			}
			obj[0] += 4.126514*m_variable_size / 2.0;
			obj[0] += m_bias;
		}
	}
}