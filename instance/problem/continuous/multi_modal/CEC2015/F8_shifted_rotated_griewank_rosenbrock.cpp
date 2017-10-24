#include "F8_shifted_rotated_griewank_rosenbrock.h"

namespace OFEC {
	namespace CEC2015 {
		F8_shifted_rotated_griewank_rosenbrock::F8_shifted_rotated_griewank_rosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			griewank_rosenbrock((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F8_shifted_rotated_griewank_rosenbrock::F8_shifted_rotated_griewank_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			griewank_rosenbrock(name, size_var, size_obj) {

			initialize();
		}

		F8_shifted_rotated_griewank_rosenbrock::~F8_shifted_rotated_griewank_rosenbrock() {
			//dtor
		}
		void F8_shifted_rotated_griewank_rosenbrock::initialize() {

			set_bias(0);

			m_translation.resize(m_variable_size);
			bool is_load = load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");  //data path
			if (!is_load) {
				std::vector<real> temp_var(m_variable_size);
				for (size_t i = 0; i < m_variable_size; ++i)
					temp_var[i] = m_original_optima.variable(0)[i];
				set_translation(temp_var);
			}
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");
			m_optima.clear();
			set_global_opt();
		}
		void F8_shifted_rotated_griewank_rosenbrock::evaluate__(real *x, vector<real>& obj) {
			griewank_rosenbrock::evaluate__(x, obj);
		}
	}
}