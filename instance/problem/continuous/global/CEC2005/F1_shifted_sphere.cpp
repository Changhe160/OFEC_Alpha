#include "F1_shifted_sphere.h"

namespace OFEC {
	namespace CEC2005 {
		F1_shifted_sphere::F1_shifted_sphere(param_map &v) :
			F1_shifted_sphere((v.at("proName")), (v.at("numDim")), 1) {


		}
		F1_shifted_sphere::F1_shifted_sphere(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			sphere(name, size_var, size_obj) {


		}

		void F1_shifted_sphere::initialize() {
			set_range(-100., 100.);
			set_original_global_opt();

			set_bias(-450);
			
			load_translation("instance/problem/continuous/global/CEC2005/data/");  //data path
			
			set_global_opt(m_translation.data());
			m_optima.set_flag_variable(true);
			m_objective_monitor = true;
			m_objective_accuracy = 1.0e-8;

			m_variable_partition.resize(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				m_variable_partition[i] = { i };
			m_initialized = true;
		}
		void F1_shifted_sphere::evaluate_objective(real *x, std::vector<real> &obj) {
			sphere::evaluate_objective(x, obj);
		}
	}
}