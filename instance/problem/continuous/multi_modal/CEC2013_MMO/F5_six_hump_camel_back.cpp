#include "F5_six_hump_camel_back.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F5_six_hump_camel_back::F5_six_hump_camel_back(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		six_hump_camel_back((v[param_proName]), (v[param_numDim]), 1) {

		}
		F5_six_hump_camel_back::F5_six_hump_camel_back(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		six_hump_camel_back(name, size_var, size_obj) {

		}

		void F5_six_hump_camel_back::evaluate__(real *x, std::vector<real>& obj) {
			six_hump_camel_back::evaluate__(x, obj);
		}
	}
}