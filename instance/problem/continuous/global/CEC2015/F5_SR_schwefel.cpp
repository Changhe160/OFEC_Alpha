
#include "F5_SR_schwefel.h"
namespace OFEC {
	namespace CEC2015 {
		F5_SR_schwefel::F5_SR_schwefel(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			schwefel((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F5_SR_schwefel::F5_SR_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			initialize();
		}

		void F5_SR_schwefel::initialize() {
			set_condition_number(1.);
			set_bias(500.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			set_scale(1. / 10.);
			m_optima.clear();
			set_global_opt(m_translation.data());

		}



		void F5_SR_schwefel::evaluate__(real *x, std::vector<real>& obj) {
			schwefel::evaluate__(x, obj);
		}
	}
}