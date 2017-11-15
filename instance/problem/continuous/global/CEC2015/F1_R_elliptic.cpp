
#include "F1_R_elliptic.h"
namespace OFEC {
	namespace CEC2015 {
		F1_R_elliptic::F1_R_elliptic(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			elliptic((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F1_R_elliptic::F1_R_elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			elliptic(name, size_var, size_obj) {

			initialize();
		}

		void F1_R_elliptic::initialize() {
			set_condition_number(1.);
			set_bias(100.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());

		}



		void F1_R_elliptic::evaluate__(real *x, std::vector<real>& obj) {
			elliptic::evaluate__(x,obj);
		}
	}
}