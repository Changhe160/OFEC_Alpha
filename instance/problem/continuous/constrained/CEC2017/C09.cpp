#include "C09.h"

namespace OFEC {
	namespace CEC2017 {
		C09::C09(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-10., 10.);
			set_init_range(-10., 10.);
			initialize();
		}
		C09::C09(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-10., 10.);
			set_init_range(-10., 10.);
			initialize();
		}

		void C09::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C09::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = *std::max_element(x, x + m_variable_size);

			obj[0] += m_bias;

			
			std::vector<real> x_1(x, x + m_variable_size);
			std::vector<real> x_2(x, x + m_variable_size);
			double temp = 0.;
			double sum1 = 0.,sum2=0.;

			std::vector<double> eq_cons;
			std::vector<double> ineq_cons;
			for (i = 1; i <= m_variable_size / 2; ++i) {
				x_1[i - 1] = x[2 * i - 2];
			}
			for (i = 0; i < m_variable_size / 2; ++i)
			{
				temp += (x_1[i] * x_1[i] - x_1[i + 1])*(x_1[i] * x_1[i] - x_1[i + 1]);
			}
			eq_cons.push_back(temp);
			temp = 0.;
			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum1 += i;
			}
			temp = 1.;
			for (i = 1; i <= m_variable_size / 2; ++i) {
				x_2[i - 1] = x[2 * i - 1];
			}
			for (i = 0; i < m_variable_size / 2; ++i)
			{
				temp *= x_2[i];
			}
			ineq_cons.push_back(temp);
			temp = 0.;

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum2 += i;
			}
			cons_values.clear();
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			for (auto &i : eq_cons)
				cons_values.push_back(i);
			cons_value = (sum1+sum2) / (double)(eq_cons.size()+ineq_cons.size());
			
		}
	}
}