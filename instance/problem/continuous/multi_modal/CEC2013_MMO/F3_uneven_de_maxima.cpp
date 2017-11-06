#include "F3_uneven_de_maxima.h"

namespace OFEC {
	namespace CEC2013_MMO {
		F3_uneven_de_maxima::F3_uneven_de_maxima(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			uneven_de_maxima((v[param_proName]), (v[param_numDim]), 1) {

		}
		F3_uneven_de_maxima::F3_uneven_de_maxima(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			uneven_de_maxima(name, size_var, size_obj) {

		}

		void F3_uneven_de_maxima::evaluate__(real *x, std::vector<real>& obj) {
			uneven_de_maxima::evaluate__(x, obj);
		}
	}
}