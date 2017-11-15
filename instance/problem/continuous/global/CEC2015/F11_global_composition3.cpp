#include "F11_global_composition3.h"
#include "../../global/classical/HGBat.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/elliptic.h"

namespace OFEC {
	namespace CEC2015 {
		F11_global_composition3::F11_global_composition3(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {
			
			initialize();
		}
		F11_global_composition3::F11_global_composition3(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			
			initialize();
		}

		void F11_global_composition3::set_function() {
			basic_func f(5);
			f[0] = &create_function<HGBat>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<schwefel>;
			f[3] = &create_function<weierstrass>;
			f[4] = &create_function<elliptic>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 10;
			m_converge_severity[2] = 10;
			m_converge_severity[3] = 20;
			m_converge_severity[4] = 20;

			m_height[0] = 10;
			m_height[1] = 10;
			m_height[2] = 2.5;
			m_height[3] = 25;
			m_height[4] = 1e-6;

			real temp = 0;
			for (auto &i : m_f_bias)
				i = temp;
			temp += 100;

		}

		void F11_global_composition3::initialize() {
			set_function();
			
			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}
			// Set optimal solution
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (int i = 0; i < m_variable_size; ++i) {
				x.get_variable()[i] = m_function[0]->get_optima().variable(0)[i];
			}
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());

			add_tag(problem_tag::MMP);
		}
		void F11_global_composition3::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<double> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function);
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> s(std::move(temp_var), std::move(temp_obj));
			for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
				s.get_variable() = x_;
				
				m_function[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.get_objective()[0];

			}
			double sumw = 0;
			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			double temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
			}

			obj[0] = temp;
			obj[0] += 1100;
		}

	}
}