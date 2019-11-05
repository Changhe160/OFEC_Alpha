#include "C24.h"

namespace OFEC {
	namespace CEC2017 {
		C24::C24(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			function((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C24::C24(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C24::initialize() {
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
		void C24::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;

			obj[0] = fabs(x[0]);
			for (i = 1; i<m_variable_size; ++i)
			{
				if (fabs(x[i]) > obj[0]) obj[0] = fabs(x[i]);
			}
			obj[0] += m_bias;

			
			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += pow(x[i], 2);
			}
			con[0] -= 100 * m_variable_size;
			if (con[0] <= 0) con[0] = 0;

			std::vector<real> x_temp(x, x + m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				x_temp[i] -= m_translation[i];
			rotate(x_temp.data());

			real result = fabs(x_temp[0]);
			for (i = 1; i<m_variable_size; ++i)
			{
				if (fabs(x_temp[i]) > result) result = fabs(x_temp[i]);
			}
			con[1] = cos(result) + sin(result);
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
			
		}
	}
}