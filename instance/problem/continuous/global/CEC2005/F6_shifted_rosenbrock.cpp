#include "F6_shifted_rosenbrock.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(param_map &v) :
			F6_shifted_rosenbrock((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F6_shifted_rosenbrock::F6_shifted_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rosenbrock(name, size_var, size_obj) {

			
		}

		void F6_shifted_rosenbrock::initialize() {
			set_range(-100, 100);
			std::vector<real> v(m_variable_size, 1);
			set_original_global_opt(v.data());
			set_bias(390);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			
			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}
		void F6_shifted_rosenbrock::evaluate_objective(real *x, std::vector<real> &obj) {

			rosenbrock::evaluate_objective(x, obj);
		}
	}
}