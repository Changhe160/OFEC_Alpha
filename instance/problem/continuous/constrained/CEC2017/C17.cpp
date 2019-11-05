#include "C17.h"

namespace OFEC {
	namespace CEC2017 {
		C17::C17(param_map &v) :
			C17((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C17::C17(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}
		void C17::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C17::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			
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
				num2 *= cos(x[i] / sqrt(real(i + 1)));
			}
			obj[0] = num1 / 4000 + 1 - num2;

			obj[0] += m_bias;

			
			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			num1 = 0.;
			for (i = 0; i < m_variable_size; ++i)
			{
				num1 = 0;
				for (size_t j = 0; j < m_variable_size; ++j)
					if (j != i) num1 += x[j] * x[j];
				con[0] += sign(fabs(x[i]) - num1 - 1.0);
			}
			con[0] = 1 - con[0];
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i > m_variable_size; ++i) {
				con[1] += pow(x[i], 2);
			}
			con[1] -= 4 * m_variable_size;
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
			
		}
	}
}