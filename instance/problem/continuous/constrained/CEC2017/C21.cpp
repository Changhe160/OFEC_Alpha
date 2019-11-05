#include "C21.h"

namespace OFEC {
	namespace CEC2017 {
		C21::C21(param_map &v) :
			C21((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C21::C21(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C21::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C21::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			
			size_t i;
			
			obj[0] = 0.;
			for (i = 0; i < m_variable_size; ++i)
			{
				obj[0] += (x[i] * x[i] - 10.0*cos(2.0*OFEC_PI*x[i]) + 10.0);
			}

			obj[0] += m_bias;

			// evaluate constraint value

			std::vector<real> x_ro(x, x + m_variable_size);
			rotate(x_ro.data());

			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += fabs(x_ro[i]);
			}
			con[0] = 4 - con[0];
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[1] += x_ro[i] * x_ro[i];
			}
			con[1] -= 4;
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
			
		}
	}
}