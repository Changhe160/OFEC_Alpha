#include "C07.h"

namespace OFEC {
	namespace CEC2017 {
		C07::C07(param_map &v) :
			C07((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C07::C07(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C07::initialize() {
			m_variable_monitor = true;
			set_range(-50., 50.);
			set_init_range(-50., 50.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C07::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size - 1; ++i)
			{
				obj[0] += x[i] * sin(x[i]);
			}
			obj[0] += m_bias;

			
			// evaluate constraint value
			
			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += (x[i] - 100 * cos(0.5*x[i]) + 100);       
			}
			
			con[1] *= -1 * con[0];

			for (auto &i : con)
				if (fabs(i) - 1e-4 <= 0) i = 0;
		}
	}
}