#include "F4_SR_expanded_decreasing_minima.h"

namespace OFEC {
	namespace CEC2015 {
		F4_SR_expanded_decreasing_minima::F4_SR_expanded_decreasing_minima(param_map &v) :
			F4_SR_expanded_decreasing_minima((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F4_SR_expanded_decreasing_minima::F4_SR_expanded_decreasing_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F4_SR_expanded_decreasing_minima::initialize() {
			set_range(-100, 100);
			set_init_range(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			set_condition_number(1.0);
			m_variable_monitor = true;
			set_bias(400.);
			set_scale(20.);
			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 5Dim  : 1 gopt and 15 lopt 
			// 10Dim : 1 gopt and 55 lopt 
			// 20Dim : 1 gopt and 210 lopt 
			evaluate_optima();
			m_initialized = true;
		}
		void F4_SR_expanded_decreasing_minima::evaluate_objective(real *x, std::vector<real> &obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;
			
			for (i = 0; i<m_variable_size; i++)
			{
				x[i] += 0.1;
				if ((x[i] <= 1.0)&(x[i] >= 0.0))
				{
					obj[0] += -exp(-2.0*log(2.0)*pow((x[i] - 0.1) / 0.8, 2.0))*pow(sin(5.0*OFEC_PI*x[i]), 6.0);
				}
				else
				{
					obj[0] += pow(x[i], 2.0);
				}
			}
		
			obj[0] += 1.0*m_variable_size;
			obj[0] += m_bias;
		}
	}
}