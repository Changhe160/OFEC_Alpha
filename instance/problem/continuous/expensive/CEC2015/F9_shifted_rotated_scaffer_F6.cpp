#include "F9_shifted_rotated_scaffer_F6.h"

namespace OFEC {
	namespace CEC2015 {
		F9_shifted_rotated_scaffer_F6::F9_shifted_rotated_scaffer_F6(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			scaffer_F6((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F9_shifted_rotated_scaffer_F6::F9_shifted_rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			scaffer_F6(name, size_var, size_obj) {

			
		}

		void F9_shifted_rotated_scaffer_F6::initialize() {
			set_tag(std::set<problem_tag>({ problem_tag::EOP, problem_tag::ConOP }));
			m_variable_monitor = true;
			set_range(-100, 100);
			set_init_range(-100, 100);
			set_original_global_opt();

			m_variable_accuracy = 1.0e-2;
			set_bias(900);

			
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt(m_translation.data());
		}
		void F9_shifted_rotated_scaffer_F6::evaluate__(real *x, std::vector<real>& obj) {
			scaffer_F6::evaluate__(x, obj);
		}
	}
}