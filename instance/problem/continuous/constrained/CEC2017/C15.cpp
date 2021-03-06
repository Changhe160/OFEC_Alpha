#include "C15.h"

namespace OFEC {
	namespace CEC2017 {
		C15::C15(param_map &v) :
			C15((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C15::C15(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}


		void C15::initialize() {
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
		void C15::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			
			std::vector<real> x_fasb(x, x + m_variable_size);
			for (auto &i : x_fasb)
				i = fabs(i);
			size_t i;
			obj[0] = *std::max_element(x_fasb.begin(), x_fasb.end());

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

			con[1] = cos(obj[0]) + sin(obj[0]);
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;

		}
	}
}