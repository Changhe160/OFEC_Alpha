
#include "F2_R_cigar.h"
namespace OFEC {
	namespace CEC2015 {
		F2_R_cigar::F2_R_cigar(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			bent_cigar((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F2_R_cigar::F2_R_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			bent_cigar(name, size_var, size_obj) {

			initialize();
		}

		void F2_R_cigar::initialize() {
			set_condition_number(1.);
			set_bias(200.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			m_optima.clear();
			set_global_opt(m_translation.data());

		}



		void F2_R_cigar::evaluate__(real *x, std::vector<real>& obj) {
			bent_cigar::evaluate__(x, obj);
		}
	}
}