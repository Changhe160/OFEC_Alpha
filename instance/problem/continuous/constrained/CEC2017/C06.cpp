#include "C06.h"

namespace OFEC {
	namespace CEC2017 {
		C06::C06(param_map &v) :
			C06((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C06::C06(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C06::initialize() {
			m_variable_monitor = true;
			set_range(-20., 20.);
			set_init_range(-20., 20.);
			m_constraint_type.resize(6);
			for (auto &i : m_constraint_type)
				i = constraint_type::Equality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C06::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size - 1; ++i)
			{
				obj[0] += (x[i] * x[i] - 10.0*cos(2.0*OFEC_PI*x[i]) + 10.0);
			}
			obj[0] += m_bias;

			
			// evaluate constraint value
			
			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				con[0] += x[i] * sin(x[i]);      //h1
			}
			con[0] *= -1;
			for (i = 0; i < m_variable_size; ++i)
			{
				con[1] += x[i] * sin(OFEC_PI*x[i]);   //h2
			}
			for (i = 0; i < m_variable_size; ++i)
			{
				con[2] += x[i] * cos(x[i]);    //h3
			}
			con[2] *= -1;
			for (i = 0; i < m_variable_size; ++i)
			{
				con[3] += x[i] * cos(OFEC_PI*x[i]);    //h4
			}
			for (i = 0; i < m_variable_size; ++i)
			{
				con[4] += x[i] * sin(2 * sqrt(fabs(x[i])));    //h5
			}
			for (i = 0; i < m_variable_size; ++i)
			{
				con[5] += x[i] * sin(2 * sqrt(fabs(x[i])));    //h6
			}
			con[5] *= -1;

			for (auto &i : con)
				if (fabs(i) - 1e-4 <= 0) i = 0;

		}
	}
}