
#include "F5_SR_schwefel.h"
namespace OFEC {
	namespace CEC2015 {
		F5_SR_schwefel::F5_SR_schwefel(param_map &v) :
			F5_SR_schwefel((v.at("proName")), (v.at("numDim")), 1) {

		
		}
		F5_SR_schwefel::F5_SR_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			
		}

		void F5_SR_schwefel::initialize() {
			m_variable_monitor = true;
			set_range(-500, 500);
			set_init_range(-500, 500);
			set_original_global_opt();
			set_condition_number(1.);
			set_bias(500.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			set_scale(1. / 10.);
			
			set_global_opt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
		}



		void F5_SR_schwefel::evaluate__(real *x, std::vector<real>& obj) {
			schwefel::evaluate__(x, obj);
		}
	}
}