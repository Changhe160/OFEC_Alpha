#include "C07.h"

namespace OFEC {
	namespace CEC2017 {
		C07::C07(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-50., 50.);
			initialize();
		}
		C07::C07(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-50., 50.);
			initialize();
		}

		void C07::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C07::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size - 1; ++i)
			{
				obj[0] += x[i] * sin(x[i]);
			}
			obj[0] += m_bias;

			
			double temp = 0.;
			double sum = 0.;

			std::vector<double> eq_cons;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x[i] - 100 * cos(0.5*x[i]) + 100);       //h1
			}
			eq_cons.push_back(temp);
			temp = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x[i] - 100 * cos(0.5*x[i]) + 100);   //h2
			}
			eq_cons.push_back(-1 * temp);
			temp = 0.;

			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum += i;
			}
			cons_values.clear();
			for (auto &i : eq_cons)
				cons_values.push_back(i);
			cons_value = sum / (double)eq_cons.size();
			

		}
	}
}