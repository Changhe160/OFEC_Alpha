#include "F1_shifted_sphere.h"

namespace OFEC {
	namespace CEC2005 {
		F1_shifted_sphere::F1_shifted_sphere(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			sphere((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F1_shifted_sphere::F1_shifted_sphere(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			sphere(name, size_var, size_obj) {

			
		}

		void F1_shifted_sphere::initialize_problem() {
			set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			set_original_global_opt();

			set_bias(-450);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			
			set_global_opt(m_translation.data());
		}
		void F1_shifted_sphere::evaluate__(real *x, std::vector<real>& obj) {
			sphere::evaluate__(x, obj);
		}
	}
}