
#include "F1_R_elliptic.h"
namespace OFEC {
	namespace CEC2015 {
<<<<<<< HEAD
		F1_R_elliptic::F1_R_elliptic(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			elliptic((v.at("proName")), (v.at("numDim")), 1) {
=======
		F1_R_elliptic::F1_R_elliptic(param_map &v) :
			F1_R_elliptic((v.at("proName")), (v.at("numDim")), 1) {
>>>>>>> zhou_li

			
		}
		F1_R_elliptic::F1_R_elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			elliptic(name, size_var, size_obj) {

			
		}

		void F1_R_elliptic::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);

			set_original_global_opt();
			set_condition_number(1.);
			set_bias(100.);
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			
			set_global_opt(m_translation.data());

		}



		void F1_R_elliptic::evaluate__(real *x, std::vector<real>& obj) {
			elliptic::evaluate__(x,obj);
		}
	}
}