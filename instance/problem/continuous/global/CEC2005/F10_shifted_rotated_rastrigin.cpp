#include "F10_shifted_rotated_rastrigin.h"

namespace OFEC {
	namespace CEC2005 {
		F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			rastrigin((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F10_shifted_rotated_rastrigin::F10_shifted_rotated_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {
		
		}

		void F10_shifted_rotated_rastrigin::initialize() {
			set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::ConOP }));
			m_variable_monitor = true;
			set_range(-5.12, 5.12);
			set_init_range(-5.12, 5.12);
			set_original_global_opt();
			set_bias(-330);
			set_condition_number(2);

			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
		}
		void F10_shifted_rotated_rastrigin::evaluate__(real *x, std::vector<real>& obj) {

			rastrigin::evaluate__(x, obj);
		}
	}
}