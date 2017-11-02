#include "C03.h"

namespace OFEC {
	namespace CEC2017 {
		C03::C03(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C03::C03(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		
		void C03::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C03::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.;
			for (i = 1; i<m_variable_size + 1; ++i)
			{
				double t = 0.0;
				for (j = 0; j<i; j++)
				{
					t += x[j];
				}
				obj[0] += t*t;
			}
			obj[0] += m_bias;
			
			double temp = 0.;
			double sum1 = 0.;
			double sum2 = 0.;
			std::vector<double> ineq_cons;
			std::vector<double> eq_cons;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x[i] * x[i] - 5000 * cos(0.1*OFEC_PI*x[i]) - 4000);
			}
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum1 += i;
			}
			temp = 0.;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x[i] * sin(0.1*OFEC_PI*x[i]);
			}
			eq_cons.push_back(-1 * temp);

			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum2 += i;
			}
			cons_values.clear();
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			for (auto &i : eq_cons)
				cons_values.push_back(i);
			cons_value = (sum1 + sum2) / (double)(ineq_cons.size() + eq_cons.size());
			
			
		}
	}
}