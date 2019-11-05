#include "five_uneven_peak_trap.h"

namespace OFEC {
	
		five_uneven_peak_trap::five_uneven_peak_trap(param_map &v) : five_uneven_peak_trap((v.at("proName"))) {

		}

		five_uneven_peak_trap::five_uneven_peak_trap(const std::string &name) : problem(name, 1, 1), function(name, 1, 1) {
			
		}

		void five_uneven_peak_trap::initialize() {
			set_range(0, 30);
			set_init_range(0, 30);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_variable_accuracy = 0.01;
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_objective_monitor = true;
			
			std::vector<std::vector<real>> obj_data(2, std::vector<real>(1, 200));
			for (auto &i : obj_data) {
				m_original_optima.append(i[0]);
			}
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void five_uneven_peak_trap::evaluate_objective(real *x, std::vector<real> &obj) {
			real s = -1.0;
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