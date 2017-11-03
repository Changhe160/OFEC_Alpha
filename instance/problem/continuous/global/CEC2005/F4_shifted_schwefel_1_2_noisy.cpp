#include "F4_shifted_schwefel_1_2_noisy.h"

namespace OFEC {
	namespace CEC2005 {
		F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			schwefel_1_2((v[param_proName]), (v[param_numDim]), 1) {

			//initialize();
		}
		F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel_1_2(name, size_var, size_obj) {

			//initialize();
		}

		void F4_shifted_schwefel_1_2_noisy::initialize() {
			set_bias(-450);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			m_optima.clear();
			set_global_opt(m_translation.data());
		}
		void F4_shifted_schwefel_1_2_noisy::evaluate__(real *x, std::vector<real>& obj) {
			schwefel_1_2::evaluate__(x, obj);
			obj[0] = (obj[0] - m_bias)*fabs(global::ms_global->m_normal[caller::Problem]->next()) + m_bias;
		}
	}
}