#include "F4_shifted_rotated_schwefel.h"

namespace OFEC {
	namespace CEC2015 {
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(param_map &v) :
			F4_shifted_rotated_schwefel((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			
		}

		void F4_shifted_rotated_schwefel::initialize() {
			m_variable_monitor = true;
			set_range(-500, 500);
			set_init_range(-500, 500);
			std::vector<real> v(m_variable_size, 420.9687);
			set_original_global_opt(v.data());
			
			m_variable_accuracy = 1.0e-2;
			set_bias(400);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt(m_translation.data());
			m_initialized = true;
		}
		void F4_shifted_rotated_schwefel::evaluate_objective(real *x, std::vector<real> &obj) {
			schwefel::evaluate_objective(x, obj);
		}
	}
}