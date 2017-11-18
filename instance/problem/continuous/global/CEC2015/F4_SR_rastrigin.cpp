
#include "F4_SR_rastrigin.h"
namespace OFEC {
	namespace CEC2015 {
		F4_SR_rastrigin::F4_SR_rastrigin(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			rastrigin((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F4_SR_rastrigin::F4_SR_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {

			initialize();
		}

		void F4_SR_rastrigin::initialize() {
			set_condition_number(1.);
			set_bias(400.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			set_scale(100. / 5.12);
			m_optima.clear();
			set_global_opt(m_translation.data());

		}



		void F4_SR_rastrigin::evaluate__(real *x, std::vector<real>& obj) {
			rastrigin::evaluate__(x, obj);
		}
	}
}