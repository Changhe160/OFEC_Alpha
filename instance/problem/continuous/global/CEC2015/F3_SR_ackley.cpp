
#include "F3_SR_ackley.h"
namespace OFEC {
	namespace CEC2015 {
		F3_SR_ackley::F3_SR_ackley(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			ackley((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F3_SR_ackley::F3_SR_ackley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			initialize();
		}

		void F3_SR_ackley::initialize() {
			set_condition_number(1.);
			set_bias(300.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());

		}



		void F3_SR_ackley::evaluate__(real *x, std::vector<real>& obj) {
			ackley::evaluate__(x, obj);
		}
	}
}