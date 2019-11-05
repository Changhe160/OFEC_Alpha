#include "C03.h"

namespace OFEC {
	namespace CEC2017 {
		C03::C03(param_map &v) :
			C03((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C03::C03(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		
		void C03::initialize() {
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
		void C03::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.;
			for (i = 1; i<m_variable_size + 1; ++i)
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

			con[0] = 0;
			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += (x[i] * x[i] - 5000 * cos(0.1*OFEC_PI*x[i]) - 4000);
			}
			if (con[0] <= 0) con[0] = 0;

			con[1] = 0;
			for (i = 0; i < m_variable_size; ++i)
			{
				con[1] += x[i] * sin(0.1*OFEC_PI*x[i]);
			}
			if (fabs(con[i]) - 1e-4 <= 0) con[1] = 0;
		}
	}
}