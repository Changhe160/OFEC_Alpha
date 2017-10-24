#include "F1_rotated_bent_cigar.h"

namespace OFEC {
	namespace CEC2015 {
		F1_rotated_bent_cigar::F1_rotated_bent_cigar(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			bent_cigar((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F1_rotated_bent_cigar::F1_rotated_bent_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			bent_cigar(name, size_var, size_obj) {

			initialize();
		}

		F1_rotated_bent_cigar::~F1_rotated_bent_cigar() {
			//dtor
		}
		void F1_rotated_bent_cigar::initialize() {

			set_bias(0);
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");
			m_optima.clear();
			set_global_opt();
		}
		void F1_rotated_bent_cigar::evaluate__(real *x, vector<real>& obj) {
			bent_cigar::evaluate__(x, obj);
		}
	}
}