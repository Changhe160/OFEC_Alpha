#include "C25.h"

namespace OFEC {
	namespace CEC2017 {
		C25::C25(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C25::C25(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}

		void C25::initialize() {
			add_tag(problem_tag::COP);
			
			load_translation("instance/problem/continuous/constrained/CEC2517/data/");  //data path
			
			load_rotation("instance/problem/continuous/constrained/CEC2517/data/");
			
		}
		void C25::evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;
			obj[0] = 0.;
			for (i = 0; i < m_variable_size; ++i) {
				obj[0] += fabs(x[i]);
			}

			obj[0] += m_bias;

			
			double temp = 0.;
			double sum1 = 0., sum2 = 0.;

			std::vector<double> eq_cons;
			std::vector<double> ineq_cons;

			temp = pow(cos(obj[0]) + sin(obj[0]), 2) - exp(cos(obj[0]) + sin(obj[0])) - 1 + exp(1.);
			eq_cons.push_back(temp);
			temp = 0.;
			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum1 += i;
			}
			temp = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += pow(x[i], 2);
			}
			temp -= 100 * m_variable_size;
			ineq_cons.push_back(temp);
			temp = 0.;

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum2 += i;
			}
			cons_values.clear();
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			for (auto &i : ineq_cons)
				cons_values.push_back(i);
			cons_value = (sum1 + sum2) / (double)(eq_cons.size() + ineq_cons.size());
			
		}
	}
}