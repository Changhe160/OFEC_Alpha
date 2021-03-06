#include "F2_rotated_discus.h"

namespace OFEC {
	namespace CEC2015 {
		F2_rotated_discus::F2_rotated_discus(param_map &v) :F2_rotated_discus((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F2_rotated_discus::F2_rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			discus(name, size_var, size_obj) {

			
		}

		void F2_rotated_discus::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);

			set_bias(200);
			set_original_global_opt();
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt();
			m_initialized = true;
		}
		void F2_rotated_discus::evaluate_objective(real *x, std::vector<real> &obj) {
			discus::evaluate_objective(x, obj);
		}
	}
}