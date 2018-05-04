#include "F2_SR_expanded_five_uneven_peak_trap.h"

namespace OFEC {
	namespace CEC2015 {
		F2_SR_expanded_five_uneven_peak_trap::F2_SR_expanded_five_uneven_peak_trap(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
			CEC2015_function((v.at("proName")), (v.at("numDim")), 1) {
			
		}
		F2_SR_expanded_five_uneven_peak_trap::F2_SR_expanded_five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F2_SR_expanded_five_uneven_peak_trap::initialize() {
			set_range(-100, 100);
			set_init_range(-100, 100);
			m_opt_mode[0] = optimization_mode::Minimization;
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			set_condition_number(1.0);
			m_variable_monitor = true;
			set_bias(200.);

			load_optima("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");

			// 2^Dim gopt and 5^Dim - 2^Dim lopt 
			evaluate_optima();

			set_tag(std::set<problem_tag>({ problem_tag::MMP, problem_tag::CONT }));
		}
		void F2_SR_expanded_five_uneven_peak_trap::evaluate__(real *x, std::vector<real>& obj) {

			size_t i;

			for (i = 0; i < m_variable_size; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_variable_size; ++i)
			{
				if (x[i]<0)
				{
					obj[0] += -200.0 + pow(x[i], 2.0);
				}
				else if (x[i]<2.5)
				{
					obj[0] += -80.0*(2.5 - x[i]);
				}
				else if (x[i]<5.0)
				{
					obj[0] += -64.0*(x[i] - 2.5);
				}
				else if (x[i]<7.5)
				{
					obj[0] += -160.0 + pow(x[i], 2.0);
				}
				else if (x[i]<12.5)
				{
					obj[0] += -28.0*(x[i] - 7.5);
				}
				else if (x[i]<17.5)
				{
					obj[0] += -28.0*(17.5 - x[i]);
				}
				else if (x[i]<22.5)
				{
					obj[0] += -32.0*(x[i] - 17.5);
				}
				else if (x[i]<27.5)
				{
					obj[0] += -32.0*(27.5 - x[i]);
				}
				else if (x[i] <= 30.0)
				{
					obj[0] += -80.0*(x[i] - 27.5);
				}
				else
				{
					obj[0] += -200.0 + pow(x[i] - 30.0, 2.0);
				}
			}
			obj[0] += 200.0*m_variable_size;
			obj[0] += m_bias;
		}
	}
}