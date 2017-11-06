#include "F7_vincent.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F7_vincent::F7_vincent(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			vincent((v[param_proName]), (v[param_numDim]), 1) {

		}
		F7_vincent::F7_vincent(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			vincent(name, size_var, size_obj) {

		}

		void F7_vincent::evaluate__(real *x, std::vector<real>& obj) {
			vincent::evaluate__(x, obj);
		}
	}
}