#include "C26.h"

namespace OFEC {
	namespace CEC2017 {
		C26::C26(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C26::C26(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		void C26::initialize() {
			add_tag(problem_tag::COP);
			//std::vector<real> data(m_variable_size, 0);
			//set_original_global_opt(data.data());
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2617/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = 0;
				set_translation(temp_var);
			}
			
			load_rotation("instance/problem/continuous/constrained/CEC2617/data/");
			//set_global_opt(m_translation.data());
		}
		void C26::evaluate__(real *x, std::vector<real>& obj, double & cons_first, std::vector<double> &cons_second) {
			
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

			
			std::vector<real> x_ro(x, x + m_variable_size);
			rotate(x_ro.data());
			double temp = 0.;
			double sum1 = 0., sum2 = 0.;

			std::vector<double> eq_cons;
			std::vector<double> ineq_cons;
			real num4 = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				num4 += x_ro[i] * x_ro[i];
			}
			temp = num4 - 4 * m_variable_size;
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
					if (j != i) num3 += x_ro[j] * x_ro[j];
				temp += sign(fabs(x_ro[i]) - num3 - 1.0);
			}
			temp = 1 - temp;
			ineq_cons.push_back(temp);
			temp = 0.;

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum2 += i;
			}
			for (auto &i : ineq_cons)
				cons_second.push_back(i);
			for (auto &i : ineq_cons)
				cons_second.push_back(i);
			cons_first = (sum1 + sum2) / (double)(eq_cons.size() + ineq_cons.size());
			
		}
	}
}