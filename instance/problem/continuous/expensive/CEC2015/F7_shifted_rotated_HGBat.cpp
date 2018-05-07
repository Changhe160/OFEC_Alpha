#include "F7_shifted_rotated_HGBat.h"

namespace OFEC {
	namespace CEC2015 {
<<<<<<< HEAD
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			HGBat((v.at("proName")), (v.at("numDim")), 1) {
=======
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(param_map &v) :
			F7_shifted_rotated_HGBat((v.at("proName")), (v.at("numDim")), 1) {
>>>>>>> zhou_li

			
		}
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			HGBat(name, size_var, size_obj) {

			
		}

		void F7_shifted_rotated_HGBat::initialize() {
			m_variable_monitor = true;
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			set_original_global_opt();
			set_bias(700);

			
			load_translation("instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			load_rotation("instance/problem/continuous/expensive/CEC2015/data/");
			
			set_global_opt(m_translation.data());
		}
		void F7_shifted_rotated_HGBat::evaluate__(real *x, std::vector<real>& obj) {
			HGBat::evaluate__(x, obj);
		}
	}
}