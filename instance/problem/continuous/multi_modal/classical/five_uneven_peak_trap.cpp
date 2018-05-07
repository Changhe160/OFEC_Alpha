#include "five_uneven_peak_trap.h"

namespace OFEC {
	
<<<<<<< HEAD
		five_uneven_peak_trap::five_uneven_peak_trap(param_map &v) :problem((v.at("proName")), 1, 1), \
			function((v.at("proName")), 1, 1) {
=======
		five_uneven_peak_trap::five_uneven_peak_trap(param_map &v) :
			five_uneven_peak_trap((v.at("proName")), 1, 1) {
>>>>>>> zhou_li

			
		}
		five_uneven_peak_trap::five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void five_uneven_peak_trap::initialize() {
			set_range(0, 30);
			set_init_range(0, 30);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			m_objective_monitor = true;
			
			std::vector<std::vector<real>> obj_data(2, std::vector<real>(1, 200));
			for (auto &i : obj_data) {
				m_original_optima.append(i[0]);
			}
			m_optima = m_original_optima;
		}
		void five_uneven_peak_trap::evaluate__(real *x, std::vector<real>& obj) {
			double s = -1.0;
			if (x[0] >= 0 && x[0] < 2.5) {
				s = 80 * (2.5 - x[0]);
			}
			else if (x[0] >= 2.5 && x[0] < 5.0) {
				s = 64 * (x[0] - 2.5);
			}
			else if (x[0] >= 5.0 && x[0] < 7.5) {
				s = 64 * (7.5 - x[0]);
			}
			else if (x[0] >= 7.5 && x[0] < 12.5) {
				s = 28 * (x[0] - 7.5);
			}
			else if (x[0] >= 12.5 && x[0] < 17.5) {
				s = 28 * (17.5 - x[0]);
			}
			else if (x[0] >= 17.5 && x[0] < 22.5) {
				s = 32 * (x[0] - 17.5);
			}
			else if (x[0] >= 22.5 && x[0] < 27.5) {
				s = 32 * (27.5 - x[0]);
			}
			else if (x[0] >= 27.5 && x[0] <= 30) {
				s = 80 * (x[0] - 27.5);
			}

			obj[0] = s + m_bias;

		}
	
}