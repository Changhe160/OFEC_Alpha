#include "F6_shifted_rosenbrock.h"

namespace OFEC {
	namespace CEC2005 {
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			rosenbrock((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rosenbrock(name, size_var, size_obj) {

			initialize();
		}

		void F6_shifted_rosenbrock::initialize() {

			set_bias(390);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F6_shifted_rosenbrock::evaluate__(real *x, std::vector<real>& obj) {

			rosenbrock::evaluate__(x, obj);
		}
	}
}