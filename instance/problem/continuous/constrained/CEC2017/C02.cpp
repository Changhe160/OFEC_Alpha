#include "C02.h"

namespace OFEC {
	namespace CEC2017 {
		C02::C02(param_map &v) :
			C02((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C02::C02(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C02::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(1);
			m_constraint_type[0] = constraint_type::Inequality;
			 
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			load_rotation("instance/problem/continuous/constrained/CEC2017/data/");

			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C02::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.;
			for (i = 1; i<m_variable_size+1 ; ++i)
			{
				real t = 0.0;
				for (j = 0; j<i; j++)
				{
					t += x[j];
				}
				obj[0] += t*t;
			}
			obj[0] += m_bias;

			// evaluate constraint value

			std::vector<real> x_(x, x + m_variable_size);
			rotate(x_.data());
			con[0] = 0;
			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += (x_[i] * x_[i] - 5000 * cos(0.1*OFEC_PI*x_[i]) - 4000);
			}
			if (con[0] <= 0) con[0] = 0;
		}
	}
}