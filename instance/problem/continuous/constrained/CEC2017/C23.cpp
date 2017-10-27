#include "C23.h"

namespace OFEC {
	namespace CEC2017 {
		C23::C23(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			constraint((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C23::C23(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			constraint(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		C23::~C23() {
			//dtor
		}
		void C23::initialize() {
			std::vector<real> data(m_variable_size, 0);
			set_original_global_opt(data.data());
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2317/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}
			m_rotation.resize(1);
			resize_rotation(m_variable_size);
			load_rotation("instance/problem/continuous/constrained/CEC2317/data/");
			set_global_opt(m_translation.data());
		}
		void C23::evaluate__(real *x, std::vector<real>& obj, double & cons) {
			std::vector<real> x_ori(x, x + m_variable_size);
			if (m_translation_flag) {
				translate(x);
				rotate(x);
				translate_origin(x);
			}

			size_t i;

			real num1 = 0., num2 = 0.;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				num1 += x[i] * x[i];
			}
			num1 = sqrt(num1 / m_variable_size);
			for (i = 0; i<m_variable_size; ++i)
			{
				num2 += cos(2 * OFEC_PI*x[i]);
			}
			num2 /= m_variable_size;
			obj[0] = -20 * exp(-0.2*num1) + 20 - exp(num2) + exp(1.0);
			obj[0] += m_bias;

			if (m_translation_flag) {
				double temp = 0.;
				double sum1 = 0., sum2 = 0.;

				std::vector<double> eq_cons;
				std::vector<double> ineq_cons;

				for (i = 0; i < m_variable_size; ++i)
				{
					temp += x[i] * x[i];
				}
				temp -= 4;
				eq_cons.push_back(temp);
				temp = 0.;
				for (auto &i : eq_cons) {
					if (fabs(i) - m_epsilon <= 0) i = 0;
					else i = fabs(i);
					sum1 += i;
				}


				for (i = 1; i < m_variable_size; ++i)
				{
					temp += pow(x[i], 2);
				}
				temp += 1 - fabs(x[0]);
				ineq_cons.push_back(temp);
				temp = 0.;

				for (auto &i : ineq_cons) {
					if (i <= 0) i = 0;
					sum2 += i;
				}

				cons = (sum1 + sum2) / (double)(eq_cons.size() + ineq_cons.size());
			}
		}
	}
}