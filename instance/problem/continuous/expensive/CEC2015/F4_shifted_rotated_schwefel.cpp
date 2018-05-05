#include "F4_shifted_rotated_schwefel.h"

namespace OFEC {
	namespace CEC2015 {
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			schwefel((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F4_shifted_rotated_schwefel::F4_shifted_rotated_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			
		}

		void F4_shifted_rotated_schwefel::initialize() {
			set_tag(std::set<problem_tag>({ problem_tag::EOP, problem_tag::ConOP }));
			m_variable_monitor = true;
			set_range(-500, 500);
			set_init_range(-500, 500);
			//vector<double> v(m_variable_size, 420.9687);
			//set_original_global_opt(v.data());
			set_original_global_opt();
			
			m_variable_accuracy = 1.0e-2;
			set_bias(400);

			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt(m_translation.data());
		}
		void F4_shifted_rotated_schwefel::evaluate__(real *x, std::vector<real>& obj) {
			schwefel::evaluate__(x, obj);
		}
	}
}