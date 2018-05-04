#include "F8_shifted_rotated_ackley_bound.h"

namespace OFEC {
	namespace CEC2005 {
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			ackley((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F8_shifted_rotated_ackley_bound::F8_shifted_rotated_ackley_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			
		}

		void F8_shifted_rotated_ackley_bound::initialize() {
			set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
			m_variable_monitor = true;
			set_range(-32.768, 32.768);
			set_init_range(-32.768, 32.768);

			set_original_global_opt();
			set_bias(-140);
			set_condition_number(100);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			load_rotation("instance/problem/continuous/global/CEC2005/data/");
			
			set_global_opt(m_translation.data());
		}
		void F8_shifted_rotated_ackley_bound::evaluate__(real *x, std::vector<real>& obj) {

			ackley::evaluate__(x, obj);
		}
	}
}