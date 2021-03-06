#include "F12_global_composition4.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/elliptic.h"
#include "../../global/classical/scaffer_F6.h"
#include "../../global/classical/happy_cat.h"

namespace OFEC {
	namespace CEC2015 {
		F12_global_composition4::F12_global_composition4(param_map &v) :
			F12_global_composition4((v.at("proName")), (v.at("numDim")), 1) {

			
		}
		F12_global_composition4::F12_global_composition4(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {

			
		}

		void F12_global_composition4::set_function() {
			basic_func f(5);
			f[0] = &create_function<schwefel>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<elliptic>;
			f[3] = &create_function<scaffer_F6>;
			f[4] = &create_function<happy_cat>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				m_function[i]->initialize();
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 20;
			m_converge_severity[2] = 20;
			m_converge_severity[3] = 30;
			m_converge_severity[4] = 30;

			m_height[0] = 0.25;
			m_height[1] = 1;
			m_height[2] = 1e-7;
			m_height[3] = 10;
			m_height[4] = 10;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 100;
			m_f_bias[3] = 200;
			m_f_bias[4] = 200;

		}

		void F12_global_composition4::initialize() {
			m_num_function = 5;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			set_range(-100., 100.);
			set_init_range(-100., 100.);
			m_variable_monitor = true;
			set_function();

			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}
			// Set optimal solution
            solution<variable_vector<real>, real> s(m_objective_size, num_constraints(), m_variable_size);
            s.variable() = m_function[0]->get_optima().variable(0);

            m_optima.append(s.variable());
            m_optima.set_flag_variable(true);

            s.evaluate(false, caller::Problem);
            m_optima.append(s.objective());

			add_tag(problem_tag::MMOP);
			m_initialized = true;
		}
		void F12_global_composition4::evaluate_objective(real *x, std::vector<real> &obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<real> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function);
            solution<variable_vector<real>, real> s(m_objective_size, num_constraints(), m_variable_size);
			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
				s.variable().vect() = x_;

				m_function[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.objective()[0];

			}
			real sumw = 0;
			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			real temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
			}

			obj[0] = temp;
			obj[0] += 1200;
		}

	}
}