#include "F3_shifted_rotated_high_cond_elliptic.h"

namespace OFEC {
	namespace CEC2005 {
		F3_shifted_rotated_high_cond_elliptic::F3_shifted_rotated_high_cond_elliptic(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			elliptic((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F3_shifted_rotated_high_cond_elliptic::F3_shifted_rotated_high_cond_elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			elliptic(name, size_var, size_obj) {

			
		}

		void F3_shifted_rotated_high_cond_elliptic::initialize() {
			set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);

			set_original_global_opt();
			set_bias(-450);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
		}
		void F3_shifted_rotated_high_cond_elliptic::evaluate__(real *x, std::vector<real>& obj) {

			elliptic::evaluate__(x, obj);
		}
	}
}