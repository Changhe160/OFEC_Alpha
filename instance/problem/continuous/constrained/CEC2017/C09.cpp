#include "C09.h"

namespace OFEC {
	namespace CEC2017 {
		C09::C09(param_map &v) :
			C09((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C09::C09(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C09::initialize() {
			m_variable_monitor = true;
			set_range(-10., 10.);
			set_init_range(-10., 10.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C09::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = *std::max_element(x, x + m_variable_size);

			obj[0] += m_bias;

			
			// evaluate constraint value

			std::vector<real> x_1(x, x + m_variable_size);
			std::vector<real> x_2(x, x + m_variable_size);

			for (auto &i : con)
				i = 0.;

			for (i = 1; i <= m_variable_size / 2; ++i) {
				x_2[i - 1] = x[2 * i - 1];
			}
			for (i = 0; i < m_variable_size / 2; ++i)
			{
				con[0] *= x_2[i];
			}
			if (con[0] <= 0) con[0] = 0;

			for (i = 1; i <= m_variable_size / 2; ++i) {
				x_1[i - 1] = x[2 * i - 2];
			}
			for (i = 0; i < m_variable_size / 2; ++i)
			{
				con[1] += (x_1[i] * x_1[i] - x_1[i + 1])*(x_1[i] * x_1[i] - x_1[i + 1]);
			}
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
		}
	}
}