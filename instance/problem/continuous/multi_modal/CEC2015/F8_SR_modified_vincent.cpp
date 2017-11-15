#include "F8_SR_modified_vincent.h"

namespace OFEC {
	namespace CEC2015 {
		F8_SR_modified_vincent::F8_SR_modified_vincent(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			CEC2015_function((v[param_proName]), (v[param_numDim]), 1) {
			initialize();
		}
		F8_SR_modified_vincent::F8_SR_modified_vincent(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			initialize();
		}

		void F8_SR_modified_vincent::initialize() {
			set_bias(800.);
			set_scale(5.);
			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 6^Dim gopt

			evaluate_optima();

			add_tag(problem_tag::MMP);
		}
		void F8_SR_modified_vincent::evaluate__(real *x, std::vector<real>& obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_variable_size; i++)
			{
				x[i] += 4.1112;//shift to orgin
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
			obj[0] += 1 + m_bias;
		}
	}
}