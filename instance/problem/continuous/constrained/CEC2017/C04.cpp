#include "C04.h"

namespace OFEC {
	namespace CEC2017 {
		C04::C04(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-10., 10.);
			initialize();
		}
		C04::C04(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-10., 10.);
			initialize();
		}

		void C04::initialize() {
			add_tag(problem_tag::COP);
		
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C04::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += (x[i] * x[i] - 10.0*cos(2.0*OFEC_PI*x[i]) + 10.0);
			}
			obj[0] += m_bias;

			
			double temp = 0.;
			double sum = 0.;

			std::vector<double> ineq_cons;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x[i] * sin(2 * x[i]);
			}
			ineq_cons.push_back(-1 * temp);
			temp = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x[i] * sin(x[i]);
			}
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum += i;
			}
			cons_values.clear();
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			cons_value = sum / (double)ineq_cons.size();
			

		}
	}
}