#include "F11_shifted_rotated_weierstrass.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(param_map &v) :
			F11_shifted_rotated_weierstrass((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F11_shifted_rotated_weierstrass::F11_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			weierstrass(name, size_var, size_obj) {
			
		}

		void F11_shifted_rotated_weierstrass::initialize() {
			set_range(-0.5, 0.5);

			set_original_global_opt();
			set_condition_number(5);
			set_bias(90);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}
		void F11_shifted_rotated_weierstrass::evaluate_objective(real *x, std::vector<real> &obj) {

			weierstrass::evaluate_objective(x, obj);
		}
	}
}