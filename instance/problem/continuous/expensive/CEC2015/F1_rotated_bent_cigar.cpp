#include "F1_rotated_bent_cigar.h"

namespace OFEC {
	namespace CEC2015 {
<<<<<<< HEAD
		F1_rotated_bent_cigar::F1_rotated_bent_cigar(param_map &v) :\
			F1_rotated_bent_cigar(v.at("proName"), v.at("numDim"), 1){
=======
		F1_rotated_bent_cigar::F1_rotated_bent_cigar(param_map &v) : F1_rotated_bent_cigar(v.at("proName"), v.at("numDim"), 1){
>>>>>>> zhou_li
					
		}
		F1_rotated_bent_cigar::F1_rotated_bent_cigar(const std::string &name, size_t size_var, size_t size_obj ) :\
			problem(name, size_var, size_obj), bent_cigar(name, size_var, size_obj) {
			
		}

		void F1_rotated_bent_cigar::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			
			set_bias(100);
			set_original_global_opt();
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt();
		}
		void F1_rotated_bent_cigar::evaluate__(real *x, std::vector<real>& obj) {
			bent_cigar::evaluate__(x, obj);
		}
	}
}