#include "C08.h"

namespace OFEC {
	namespace CEC2017 {
		C08::C08(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-100., 100.);
			initialize();
		}
		C08::C08(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			set_range(-100., 100.);
			initialize();
		}
		void C08::initialize() {
			add_tag(problem_tag::COP);
			//std::vector<real> data(m_variable_size, 0);
			//set_original_global_opt(data.data());
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}

			//set_global_opt(m_translation.data());
		}
		void C08::evaluate__(real *x, std::vector<real>& obj, double & cons_first, std::vector<double> &cons_second) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i,j;
			obj[0] = *max_element(x,x+m_variable_size);
			
			obj[0] += m_bias;

			
			std::vector<real> x_1(x, x + m_variable_size);
			std::vector<real> x_2(x, x + m_variable_size);
			double temp = 0.;
			double sum = 0.;

			std::vector<double> eq_cons;
			for (i = 1; i <= m_variable_size/2; ++i) {
				x_1[i-1] = x[2 * i - 2];
			}
			for (i = 0; i < m_variable_size/2 ; ++i)
			{	
				real m = 0.;
				for (j = 0; j<i+1; j++)    //h1
				{
					m += x_1[j];
				}
				temp += m*m;       
			}
			eq_cons.push_back(temp);
			temp = 0.;

			for (i = 1; i <= m_variable_size / 2; ++i) {
				x_2[i - 1] = x[2 * i - 1];
			}
			for (i = 0; i < m_variable_size / 2; ++i)
			{
				real m = 0.;
				for (j = 0; j<i + 1; j++)    //h2
				{
					m += x_2[j];
				}
				temp += m*m;       
			}
			eq_cons.push_back(temp);
			temp = 0.;

			for (auto &i : eq_cons) {
				if (fabs(i) - 1e-4 <= 0) i = 0;
				else i = fabs(i);
				sum += i;
			}
			for (auto &i : eq_cons)
				cons_second.push_back(i);
			cons_first = sum / (double)eq_cons.size();
			
		}
	}
}