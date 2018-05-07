#include "F6_shifted_rosenbrock.h"

namespace OFEC {
	namespace CEC2005 {
<<<<<<< HEAD
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			rosenbrock((v.at("proName")), (v.at("numDim")), 1) {
=======
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(param_map &v) :
			F6_shifted_rosenbrock((v.at("proName")), (v.at("numDim")), 1) {
>>>>>>> zhou_li

			
		}
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rosenbrock(name, size_var, size_obj) {

			
		}

		void F6_shifted_rosenbrock::initialize() {
			m_variable_monitor = true;
			set_range(-2.048, 2.048);
			set_init_range(-2.048, 2.048);
			std::vector<double> v(m_variable_size, 1);
			set_original_global_opt(v.data());
			set_bias(390);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			
			set_global_opt(m_translation.data());
		}
		void F6_shifted_rosenbrock::evaluate__(real *x, std::vector<real>& obj) {

			rosenbrock::evaluate__(x, obj);
		}
	}
}