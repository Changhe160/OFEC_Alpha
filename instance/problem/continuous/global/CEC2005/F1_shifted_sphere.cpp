#include "F1_shifted_sphere.h"

namespace OFEC {
	namespace CEC2005 {
		F1_shifted_sphere::F1_shifted_sphere(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			sphere((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F1_shifted_sphere::F1_shifted_sphere(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			sphere(name, size_var, size_obj) {

			initialize();
		}

		void F1_shifted_sphere::initialize() {

			set_bias(-450);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F1_shifted_sphere::evaluate__(real *x, std::vector<real>& obj) {
			sphere::evaluate__(x, obj);
		}
	}
}