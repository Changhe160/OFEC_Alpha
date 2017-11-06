#include "F8_modified_rastrigin.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F8_modified_rastrigin::F8_modified_rastrigin(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			modified_rastrigin((v[param_proName]), (v[param_numDim]), 1) {

		}
		F8_modified_rastrigin::F8_modified_rastrigin(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			modified_rastrigin(name, size_var, size_obj) {

		}

		void F8_modified_rastrigin::evaluate__(real *x, std::vector<real>& obj) {
			modified_rastrigin::evaluate__(x, obj);
		}
	}
}