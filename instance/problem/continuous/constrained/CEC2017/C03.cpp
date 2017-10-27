#include "C03.h"

namespace OFEC {
	namespace CEC2017 {
		C03::C03(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			constraint((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C03::C03(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			constraint(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		C03::~C03() {
			//dtor
		}
		void C03::initialize() {
			set_original_global_opt();
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}
			
			set_global_opt(m_translation.data());
		}
		void C03::evaluate__(real *x, std::vector<real>& obj, double & cons) {
			if (m_translation_flag) {
				translate(x);
				translate_origin(x);
			}

			size_t i, j;
			obj[0] = 0.;
			for (i = 1; i<m_variable_size + 1; ++i)
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

			double temp = 0.;
			double sum1 = 0.;
			double sum2 = 0.;
			std::vector<double> ineq_cons;
			std::vector<double> eq_cons;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += (x_[i] * x_[i] - 5000 * cos(0.1*OFEC_PI*x_[i]) - 4000);
			}
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum1 += i;
			}
			temp = 0.;
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x_[i] * sin(0.1*OFEC_PI*x_[i]);
			}
			eq_cons.push_back(-1*temp);

			for (auto &i : eq_cons) {
				if (fabs(i)- m_epsilon <= 0) i = 0;
				else i = fabs(i);
				sum2 += i;
			}

			cons = (sum1 + sum2) / (double)(ineq_cons.size() + eq_cons.size());
			
		}
	}
}