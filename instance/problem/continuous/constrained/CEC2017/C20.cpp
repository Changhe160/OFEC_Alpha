#include "C20.h"

namespace OFEC {
	namespace CEC2017 {
		C20::C20(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C20::C20(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		void C20::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
		}
		void C20::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			

			size_t i;
			real num1 = 0.;
			obj[0] = 0.;

			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += 0.5 + (pow(sin(pow((x[i] * x[i] + x[i + 1] * x[i + 1]), 0.5)), 2) - 0.5) / pow(1 + 0.001*pow(x[i] * x[i] + x[i + 1] * x[i + 1], 0.5), 2);
			}
			auto n = m_variable_size-1;
			obj[0] += 0.5 + (pow(sin(pow((x[n] * x[n] + x[0] * x[0]), 0.5)), 2) - 0.5) / pow(1 + 0.001*pow(x[n] * x[n] + x[0] * x[0], 0.5), 2);
			obj[0] += m_bias;

			double temp = 0.;
			double sum = 0.;

			std::vector<double> ineq_cons;
			for (i = 0; i < m_variable_size; ++i)
				num1 += x[i];
				
			temp = pow(cos(num1), 2) - 0.25*cos(num1) - 0.125;
		
			ineq_cons.push_back(temp);
				
			temp = exp(cos(num1)) - exp(0.25);
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