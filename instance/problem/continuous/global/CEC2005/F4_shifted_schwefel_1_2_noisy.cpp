#include "F4_shifted_schwefel_1_2_noisy.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(param_map &v) :
			F4_shifted_schwefel_1_2_noisy((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F4_shifted_schwefel_1_2_noisy::F4_shifted_schwefel_1_2_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel_1_2(name, size_var, size_obj) {

			
		}

		void F4_shifted_schwefel_1_2_noisy::initialize() {
			set_range(-100, 100);
			set_original_global_opt();
			set_bias(-450);
			
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
		void F4_shifted_schwefel_1_2_noisy::evaluate_objective(real *x, std::vector<real> &obj) {
			schwefel_1_2::evaluate_objective(x, obj);
			obj[0] = (obj[0] - m_bias)*fabs(global::ms_global->m_normal[caller::Problem]->next()) + m_bias;
		}
	}
}