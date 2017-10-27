#include "C04.h"

namespace OFEC {
	namespace CEC2017 {
		C04::C04(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			constraint((v[param_proName]), (v[param_numDim]), 1) {
			set_range(-10., 10.);
			initialize();
		}
		C04::C04(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			constraint(name, size_var, size_obj) {
			set_range(-10., 10.);
			initialize();
		}

		C04::~C04() {
			//dtor
		}
		void C04::initialize() {
			set_original_global_opt();
			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/constrained/CEC2017/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}

			set_global_opt(m_translation.data());
		}
		void C04::evaluate__(real *x, std::vector<real>& obj, double & cons) {
			if (m_translation_flag) {
				translate(x);
				translate_origin(x);
			}

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_variable_size; ++i)
			{
				obj[0] += (x[i] * x[i] - 10.0*cos(2.0*OFEC_PI*x[i]) + 10.0);
			}
			obj[0] += m_bias;

			std::vector<real> x_(x, x + m_variable_size);

			double temp = 0.;
			double sum = 0.;
			
			std::vector<double> ineq_cons;
			
			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x_[i] * sin(2 * x_[i]);
			}
			ineq_cons.push_back(-1 * temp);
			temp = 0.;

			for (i = 0; i < m_variable_size; ++i)
			{
				temp += x_[i] * sin(x_[i]);
			}
			ineq_cons.push_back(temp);

			for (auto &i : ineq_cons) {
				if (i <= 0) i = 0;
				sum += i;
			}

			cons = sum / (double)ineq_cons.size();

		}
	}
}