#include "shubert.h"

namespace OFEC {
	
	shubert::shubert(param_map &v) :
		shubert((v.at("proName")), (v.at("numDim")), 1) {

		
	}
	shubert::shubert(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void shubert::initialize() {
		set_range(-10, 10);
		set_init_range(-10, 10);
		m_variable_accuracy = 0.5;
		m_objective_monitor = true;
		size_t num = m_variable_size*(size_t)pow(3, m_variable_size);
		
		if (m_variable_size == 2) {
			m_objective_accuracy = 1.e-4;

			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-186.7309);
			}
		}
		else if (m_variable_size == 3) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-2709.09);
			}
		}
		else if (m_variable_size == 4) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-39303.6);
			}
		}
		else {

		}
		
		m_optima = m_original_optima;
		
	}
	void shubert::evaluate__(real *x, std::vector<real>& obj) {
		double s = 1;

		for (int j = 0; j < m_variable_size; ++j) {
			double a = 0;
			for (int i = 1; i <= 5; i++) {
				a += i*cos((i + 1)*x[j] + i);
			}
			s *= a;
		}
		obj[0] = s + m_bias;

	}
	
}