#include "C05.h"

namespace OFEC {
	namespace CEC2017 {
		C05::C05(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			constraint((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-10., 10.);
			initialize();
		}
		C05::C05(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			constraint(name, size_var, size_obj) {
			set_range(-10., 10.);
			initialize();
		}

		C05::~C05() {
			//dtor
		}
		void C05::initialize() {
			std::vector<real> data(m_variable_size, 1);
			set_original_global_opt(data.data());
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}
			m_rotation.resize(2);
			resize_rotation(m_variable_size);
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");
			//set_rotation();
			set_global_opt(m_translation.data());
		}
		void C05::evaluate__(real *x, std::vector<real>& obj, double & cons) {
			if (m_translation_flag) {
				translate(x);
				translate_origin(x);
			}

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size - 1; ++i)
			{
				obj[0] += (100 * (x[i] * x[i] - x[i + 1])*(x[i] * x[i] - x[i + 1]) + (x[i] - 1)*(x[i] - 1));
			}
			obj[0] += m_bias;

			if (m_rotation_flag) {
				std::vector<real> x_1(x, x + m_variable_size);
				std::vector<real> x_2(x, x + m_variable_size);
				double temp = 0.;
				double sum = 0.;

				std::vector<double> ineq_cons;
				rotate(x_1.data(), 1);   // rotate x_1 using rotation matrix 1
				for (i = 0; i < m_variable_size; ++i)
				{
					temp += (x_1[i] * x_1[i] - 50.0*cos(2.0*OFEC_PI*x_1[i]) - 40.0);
				}
				ineq_cons.push_back(temp);
				temp = 0.;
				rotate(x_2.data(), 2);  // rotate x_2 using rotation matrix 2
				for (i = 0; i < m_variable_size; ++i)
				{
					temp += (x_2[i] * x_2[i] - 50.0*cos(2.0*OFEC_PI*x_2[i]) - 40.0);
				}
				ineq_cons.push_back(temp);

				for (auto &i : ineq_cons) {
					if (i <= 0) i = 0;
					
					sum += i;
				}

				cons = sum / (double)ineq_cons.size();
			}
		}
	}
}