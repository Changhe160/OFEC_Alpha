#include "C01.h"

namespace OFEC {
	namespace CEC2017 {
		C01::C01(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C01::C01(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		void C01::initialize() {
			add_tag(problem_tag::COP);
		
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path 
			
		}
		void C01::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.0;
			for (i = 1; i<m_variable_size+1; ++i)
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
			double sum = 0.;
			std::vector<double> constraint;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x[i] * x[i] - 5000 * cos(0.1*OFEC_PI*x[i]) - 4000);
			}
			constraint.push_back(temp);
			for (auto &i : constraint) {
				if (i <= 0) i = 0;
				sum += i;
			}
			cons_values.clear();
			for (auto &i : constraint)
				cons_values.push_back(i);
			cons_value = sum / (double)constraint.size();
			
		}
	}
}