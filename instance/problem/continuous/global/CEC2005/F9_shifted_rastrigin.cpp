#include "F9_shifted_rastrigin.h"

namespace OFEC {
	namespace CEC2005 {
		F9_shifted_rastrigin::F9_shifted_rastrigin(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			rastrigin((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F9_shifted_rastrigin::F9_shifted_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {
		
		}

		void F9_shifted_rastrigin::initialize_problem() {
			set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
			m_variable_monitor = true;
			set_range(-5.12, 5.12);
			set_init_range(-5.12, 5.12);
			set_original_global_opt();
			set_bias(-330);

			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path

			
			set_global_opt(m_translation.data());
		}
		void F9_shifted_rastrigin::evaluate__(real *x, std::vector<real>& obj) {

			rastrigin::evaluate__(x, obj);
		}
	}
}