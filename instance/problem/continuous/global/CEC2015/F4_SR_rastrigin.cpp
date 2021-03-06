
#include "F4_SR_rastrigin.h"
namespace OFEC {
	namespace CEC2015 {
		F4_SR_rastrigin::F4_SR_rastrigin(param_map &v) :
			F4_SR_rastrigin((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F4_SR_rastrigin::F4_SR_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {

			
		}

		void F4_SR_rastrigin::initialize() {
			m_variable_monitor = true;
			set_range(-5.12, 5.12);
			set_init_range(-5.12, 5.12);
			set_original_global_opt();
			set_condition_number(1.);
			set_bias(400.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			set_scale(100. / 5.12);
			
			set_global_opt(m_translation.data());
			m_initialized = true;
		}



		void F4_SR_rastrigin::evaluate_objective(real *x, std::vector<real> &obj) {
			rastrigin::evaluate_objective(x, obj);
		}
	}
}