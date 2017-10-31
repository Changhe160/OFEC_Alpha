#include "C28.h"

namespace OFEC {
	namespace CEC2017 {
		C28::C28(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-50., 50.);
			initialize();
		}
		C28::C28(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-50., 50.);
			initialize();
		}

		void C28::initialize() {
			add_tag(problem_tag::COP);
			//std::vector<real> data(m_variable_size, 0);
			//set_original_global_opt(data.data());
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2817/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = 0;
				set_translation(temp_var);
			}
			
			load_rotation("instance/problem/continuous/constrained/CEC2817/data/");
			//set_global_opt(m_translation.data());
		}
		void C28::evaluate__(real *x, std::vector<real>& obj, double & cons_first, std::vector<double> &cons_second) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;
			obj[0] = 0.;

			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += (pow(fabs(x[i]), 0.5) + 2 * sin(pow(x[i], 3)));
			}
			obj[0] += m_bias;

			
			double temp = 0.;
			double sum = 0.;

			std::vector<double> ineq_cons;

			for (i = 0; i<m_variable_size - 1; ++i)
			{
				temp += -10 * exp(-0.2*sqrt(x[i] * x[i] + x[i + 1] * x[i + 1]));
			}
			temp += (m_variable_size - 1) * 10 / exp(-5.0);
			ineq_cons.push_back(temp);
			temp = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				temp += sin(2 * x[i])*sin(2 * x[i]);
			}
			temp -= 0.5 * m_variable_size;
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum += i;
			}
			for (auto &i : ineq_cons)
				cons_second.push_back(i);
			cons_first = sum / (double)ineq_cons.size();
			
		}
	}
}