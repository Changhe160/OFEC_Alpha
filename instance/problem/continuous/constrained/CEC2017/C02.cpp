#include "C02.h"

namespace OFEC {
	namespace CEC2017 {
		C02::C02(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C02::C02(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}
		void C02::initialize() {
			add_tag(problem_tag::COP);
			//set_original_global_opt();
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = 0;
				set_translation(temp_var);
			}
			
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");
			//set_global_opt(m_translation.data());
		}
		void C02::evaluate__(real *x, std::vector<real>& obj, double & cons_first, std::vector<double> &cons_second) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.;
			for (i = 1; i<m_variable_size+1 ; ++i)
			{
				double t = 0.0;
				for (j = 0; j<i; j++)
				{
					t += x[j];
				}
				obj[0] += t*t;
			}
			obj[0] += m_bias;
			
			std::vector<real> x_(x, x + m_variable_size);
			rotate(x_.data());

			double temp = 0.;
			double sum = 0.;
			std::vector<double> constraint;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x_[i] * x_[i] - 5000 * cos(0.1*OFEC_PI*x_[i]) - 4000);
			}
			constraint.push_back(temp);
			for (auto &i : constraint) {
				if (i <= 0) i = 0;
				sum += i;
			}
			for (auto &i : constraint)
				cons_second.push_back(i);
			cons_first = sum / (double)constraint.size();
			
		}
	}
}