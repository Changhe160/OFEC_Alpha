#include "C22.h"

namespace OFEC {
	namespace CEC2017 {
		C22::C22(param_map &v) :
			C22((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C22::C22(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C22::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(3);
			for (auto &i : m_constraint_type)
				i = constraint_type::Inequality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C22::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			std::vector<real> x_ori(x, x + m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;

			obj[0] = 0.;
			for (i = 0; i<m_variable_size - 1; ++i)
			{
				obj[0] += (100 * (x[i] * x[i] - x_ori[i + 1])*(x[i] * x[i] - x_ori[i + 1]) + (x[i] - 1)*(x[i] - 1));
			}
			obj[0] += m_bias;

			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += x[i] * x[i] - 10 * cos(2 * OFEC_PI*x[i]) + 10;
			}
			con[0] -= 100;
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[1] += x[i];
			}
			con[1] -= 2 * m_variable_size;
			if (con[1] <= 0) con[1] = 0;
			
			for (i = 0; i < m_variable_size; ++i)
			{
				con[2] += x[i];
			}
			con[2] = 5 - con[2];
			if (con[2] <= 0) con[2] = 0;
		}
	}
}