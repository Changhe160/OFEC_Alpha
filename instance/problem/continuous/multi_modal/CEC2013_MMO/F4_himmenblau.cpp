#include "F4_himmenblau.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F4_himmenblau::F4_himmenblau(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		himmenblau((v[param_proName]), (v[param_numDim]), 1) {

		}
		F4_himmenblau::F4_himmenblau(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		himmenblau(name, size_var, size_obj) {

		}

		void F4_himmenblau::evaluate__(real *x, std::vector<real>& obj) {
			himmenblau::evaluate__(x, obj);
		}
	}
}