#include "C20.h"

namespace OFEC {
	namespace CEC2017 {
		C20::C20(param_map &v) :
			C20((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		C20::C20(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C20::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Inequality;
			 
			
			load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			set_original_global_opt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C20::evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) {
			for (size_t i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			

			size_t i;
			obj[0] = 0.;

			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += 0.5 + (pow(sin(pow((x[i] * x[i] + x[i + 1] * x[i + 1]), 0.5)), 2) - 0.5) / pow(1 + 0.001*pow(x[i] * x[i] + x[i + 1] * x[i + 1], 0.5), 2);
			}
			auto n = m_variable_size-1;
			obj[0] += 0.5 + (pow(sin(pow((x[n] * x[n] + x[0] * x[0]), 0.5)), 2) - 0.5) / pow(1 + 0.001*pow(x[n] * x[n] + x[0] * x[0], 0.5), 2);
			obj[0] += m_bias;

			// evaluate constraint value
			
			for (auto &i : con)
				i = 0.;

			real num1 = 0.;
			for (i = 0; i < m_variable_size; ++i)
				num1 += x[i];

			con[0] = pow(cos(num1), 2) - 0.25*cos(num1) - 0.125;
			if (con[0] <= 0) con[0] = 0;

			con[1] = exp(cos(num1)) - exp(0.25);
			if (con[1] <= 0) con[1] = 0;

		}
	}
}