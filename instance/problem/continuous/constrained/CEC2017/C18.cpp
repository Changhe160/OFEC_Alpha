#include "C18.h"

namespace OFEC {
	namespace CEC2017 {
		C18::C18(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C18::C18(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		void C18::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C18::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {

			
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			
			
			std::vector<real> x_(x, x + m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i) {
				if (fabs(x[i] >= 0.5)) x_[i] = 0.5*round(2 * x[i]);
			}
			size_t i;
			obj[0] = 0.;

			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += x_[i] * x_[i] - 10 * cos(2 * OFEC_PI*x_[i]) + 10;
			}
			obj[0] += m_bias;

			
			double temp = 0.;
			double sum1 = 0., sum2 = 0.;

			std::vector<double> eq_cons;
			std::vector<double> ineq_cons;
			real num1 = 0., num2 = 1.;
			for (i = 0; i<m_variable_size-1; ++i)
			{
				num1 += 100 * pow((pow(x[i], 2) - x[i + 1]), 2);
				num2 *= pow(sin(x[i] - 1), 2)*OFEC_PI;
			}

			temp = num1 + num2;
			eq_cons.push_back(temp);

			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum1 += i;
			}
			temp = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				temp += fabs(x[i]);
			}
			temp = 1 - temp;
			ineq_cons.push_back(temp);
			temp = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				temp += pow(x[i],2);
			}
			temp -= 100*m_variable_size;
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum2 += i;
			}
			cons_values.clear();
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			for (auto &i : eq_cons)
				cons_values.push_back(i);
			cons_value = (sum1 + sum2) / (double)(eq_cons.size() + ineq_cons.size());
			
		}
	}
}