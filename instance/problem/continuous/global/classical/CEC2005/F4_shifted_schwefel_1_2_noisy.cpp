#include "F4_shifted_schwefel_1_2_noisy.h"

namespace OFEC {
	F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		F2_shifted_schwefel_1_2((v[param_proName]), (v[param_numDim]), 1) {

		//initialize();
	}
	F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		F2_shifted_schwefel_1_2(name, size_var, size_obj) {

		//initialize();
	}

	F4_shifted_schwefel_1_2_noisy::~F4_shifted_schwefel_1_2_noisy() {
		//dtor
	}
	void F4_shifted_schwefel_1_2_noisy::initialize() {

	}
	void F4_shifted_schwefel_1_2_noisy::evaluate__(real *x, vector<real>& obj) {
		F2_shifted_schwefel_1_2::evaluate__(x, obj);
		obj[0] = (obj[0] - m_bias)*fabs(global::ms_global->m_normal[caller::Problem]->next()) + m_bias;
	}
}