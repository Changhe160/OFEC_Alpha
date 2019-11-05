#include "C19.h"

namespace OFEC {
	namespace CEC2017 {
		C19::C19(param_map &v) :
			C19((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C19::C19(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C19::initialize() {
			m_variable_monitor = true;
			set_range(-50., 50.);
			set_init_range(-50., 50.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Inequality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C19::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			
			size_t i;
			obj[0] = 0.;

			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += (pow(fabs(x[i]), 0.5) + 2 * sin(pow(x[i], 3)));
			}
			obj[0] += m_bias;

			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i<m_variable_size - 1; ++i)
			{
				con[0] += -10 * exp(-0.2*sqrt(x[i] * x[i] + x[i + 1] * x[i + 1]));
			}
			con[0] += (m_variable_size - 1) * 10 / exp(-5.0);
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i<m_variable_size; ++i)
			{
				con[1] += sin(2 * x[i])*sin(2 * x[i]);
			}
			con[1] -= 0.5 * m_variable_size;
			if (con[1] <= 0) con[1] = 0;
			
		}
	}
}