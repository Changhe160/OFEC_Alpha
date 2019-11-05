#include "C10.h"

namespace OFEC {
	namespace CEC2017 {
		C10::C10(param_map &v) :
			C10((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C10::C10(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C10::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C10::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i,j;
			obj[0] = *std::max_element(x, x + m_variable_size);

			obj[0] += m_bias;

			
			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 1; i < m_variable_size + 1; ++i)
			{
				real m = 0.;

				for (j = 0; j<i; j++)
				{
					m += x[j];
				}
				con[0] += m*m;
			}

			for (i = 0; i < m_variable_size - 1; ++i)
			{
				con[1] += pow((x[i] - x[i + 1]), 2);
			}

			for (auto &i : con)
				if (fabs(i) - 1e-4 <= 0) i = 0;
		}
	}
}