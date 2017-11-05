#include "C17.h"

namespace OFEC {
	namespace CEC2017 {
		C17::C17(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C17::C17(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}
		void C17::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C17::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			

			size_t i;
			obj[0] = 0.;
			real num1 = 0., num2 = 1.;
			
			for (i = 0; i<m_variable_size; ++i)
			{
				num1 += x[i] * x[i];
			}
			for (i = 0; i<m_variable_size; ++i)
			{
				num2 *= cos(x[i] / sqrt(double(i + 1)));
			}
			obj[0] = num1 / 4000 + 1 - num2;

			obj[0] += m_bias;

			
			double temp = 0.;
			double sum1 = 0., sum2 = 0.;

			std::vector<double> eq_cons;
			std::vector<double> ineq_cons;

			temp = num1 - 4*m_variable_size;
			eq_cons.push_back(temp);
				
			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum1 += i;
			}
			temp = 0.;
			real num3 = 0.;
			for (i = 0; i < m_variable_size; ++i)
			{
				num3 = 0;
				for (size_t j = 0; j < m_variable_size; ++j)
					if(j != i) num3 += x[j]*x[j];
				temp += sign(fabs(x[i]) - num3 - 1.0);
			}
			temp = 1 - temp;
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
			cons_value = (sum1 + sum2) / (double)(eq_cons.size() + ineq_cons.size());
			
		}
	}
}