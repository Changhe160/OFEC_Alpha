#include "C02.h"

namespace OFEC {
	namespace CEC2017 {
		C02::C02(param_map &v) :
			C02((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C02::C02(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C02::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);

			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
		}
		void C02::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
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
			cons_values.clear();
			for (auto &i : constraint)
				cons_values.push_back(i);
			cons_value = sum / (double)constraint.size();
			
		}
	}
}