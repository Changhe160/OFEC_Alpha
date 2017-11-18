#include "F6_SR_expanded_himmelblau.h"

namespace OFEC {
	namespace CEC2015 {
		F6_SR_expanded_himmelblau::F6_SR_expanded_himmelblau(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			CEC2015_function((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F6_SR_expanded_himmelblau::F6_SR_expanded_himmelblau(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			initialize();
		}

		void F6_SR_expanded_himmelblau::initialize() {
			set_bias(600.);
			set_scale(5.);
			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 2^Dim gopt

			evaluate_optima();

			add_tag(problem_tag::MMP);
		}
		void F6_SR_expanded_himmelblau::evaluate__(real *x, std::vector<real>& obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_variable_size - 1; i = i + 2)
			{
				x[i] += 3.0;
				x[i + 1] += 2.0;//shift to orgin
				obj[0] += pow((x[i] * x[i] + x[i + 1] - 11.0), 2.0) + pow((x[i] + x[i + 1] * x[i + 1] - 7.0), 2.0);
			}
			obj[0] += m_bias;
		}
	}
}