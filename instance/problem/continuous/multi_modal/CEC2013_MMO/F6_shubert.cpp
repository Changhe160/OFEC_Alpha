#include "F6_shubert.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F6_shubert::F6_shubert(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		shubert((v[param_proName]), (v[param_numDim]), 1) {

		}
		F6_shubert::F6_shubert(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		shubert(name, size_var, size_obj) {

		}

		void F6_shubert::evaluate__(real *x, std::vector<real>& obj) {
			shubert::evaluate__(x, obj);
		}
	}
}