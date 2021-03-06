
#include "F3_SR_ackley.h"
namespace OFEC {
	namespace CEC2015 {
		F3_SR_ackley::F3_SR_ackley(param_map &v) :
			F3_SR_ackley((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F3_SR_ackley::F3_SR_ackley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			
		}

		void F3_SR_ackley::initialize() {
			m_variable_monitor = true;
			set_range(-32.768, 32.768);
			set_init_range(-32.768, 32.768);

			set_original_global_opt();
			set_condition_number(1.);
			set_bias(300.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			
			set_global_opt(m_translation.data());
			m_initialized = true;
		}



		void F3_SR_ackley::evaluate_objective(real *x, std::vector<real> &obj) {
			ackley::evaluate_objective(x, obj);
		}
	}
}