#include "F13_global_composition5.h"
#include "F8_hybrid3.h"
#include "../../global/classical/rastrigin.h"
#include "F6_hybrid1.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/scaffer_F6.h"

namespace OFEC {
	namespace CEC2015 {
		F13_global_composition5::F13_global_composition5(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 3;
			m_num_hybrid = 2;
			m_function.resize(m_num_function);
			m_hybrid.resize(m_num_hybrid);
			m_height.resize(m_num_function + m_num_hybrid);
			m_converge_severity.resize(m_num_function + m_num_hybrid);
			m_f_bias.resize(m_num_function + m_num_hybrid);
			initialize();
		}
		F13_global_composition5::F13_global_composition5(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			m_num_function = 3;
			m_num_hybrid = 2;
			m_function.resize(m_num_function);
			m_hybrid.resize(m_num_hybrid);
			m_height.resize(m_num_function + m_num_hybrid);
			m_converge_severity.resize(m_num_function + m_num_hybrid);
			m_f_bias.resize(m_num_function + m_num_hybrid);
			initialize();
		}

		F13_global_composition5::~F13_global_composition5() {
			if (m_num_function > 0) {
				for (auto &i : m_hybrid)
					if (i) delete i;
			}
		}
		void F13_global_composition5::set_function() {
			basic_func f(5);
			f[0] = &create_function<F8_hybrid3>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<F6_hybrid1>;
			f[3] = &create_function<schwefel>;
			f[4] = &create_function<scaffer_F6>;

			size_t num1 = 0;
			size_t num2 = 0;
			for (size_t i = 0; i < m_num_function + m_num_hybrid; ++i) {
				if(i==0||i==2) m_hybrid[num1++] = dynamic_cast<hybrid*>(f[i]("", m_variable_size, m_objective_size));
				else m_function[num2++] = dynamic_cast<function*>(f[i]("", m_variable_size, m_objective_size));
				//m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 10;
			m_converge_severity[2] = 10;
			m_converge_severity[3] = 20;
			m_converge_severity[4] = 20;

			m_height[0] = 1;
			m_height[1] = 10;
			m_height[2] = 1;
			m_height[3] = 25;
			m_height[4] = 10;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;
			m_f_bias[3] = 300;
			m_f_bias[4] = 400;

		}

		void F13_global_composition5::initialize() {
			set_function();

			load_translation("instance/problem/continuous/global/CEC2015/data/");
			load_hybrid_translation("instance/problem/continuous/global/CEC2015/data/");
			load_rotation("instance/problem/continuous/global/CEC2015/data/");

			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			size_t num = 0;
			std::vector<real> hy_bias = { 800,600 };
			for (auto &i : m_hybrid) {
				for (size_t j = 0; j < m_variable_size; ++j)
					temp_var[j] = i->get_optima().variable(0)[j] + i->hybrid_translation()[j];
				i->get_optima().clear();
				solution<variable<real>, real> x(temp_var, temp_obj);
				i->get_optima().append(x.get_variable());
				i->evaluate_(x, caller::Problem, false, false);
				x.get_objective()[0] -= hy_bias[num++];
				i->get_optima().append(x.get_objective());
			}
			// Set optimal solution
			
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (int i = 0; i < m_variable_size; ++i) {
				x.get_variable()[i] = m_hybrid[0]->get_optima().variable(0)[i];
			}
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());

			add_tag(problem_tag::MMP);
		}
		void F13_global_composition5::evaluate__(real *x, std::vector<real>& obj) {
			std::vector<real> x_(m_variable_size);
			std::copy(x, x + m_variable_size, x_.begin());
			std::vector<double> weight(m_num_function + m_num_hybrid, 0);

			set_weight(weight, x_);
			std::vector<real> fit(m_num_function + m_num_hybrid);
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> s(std::move(temp_var), std::move(temp_obj));
			size_t num1 = 0;
			size_t num2 = 0;
			std::vector<real> hy_bias = { 800,600 };
			for (size_t i = 0; i < m_num_function + m_num_hybrid; ++i) { // calculate objective value for each function
				s.get_variable() = x_;

				if (i == 0 || i == 2) {
					for (size_t j = 0; j < m_variable_size; ++j)
						s.get_variable()[j] -= m_hybrid[num1]->hybrid_translation()[j];
					m_hybrid[num1]->evaluate_(s, caller::Problem, false, false);
					fit[i] = s.get_objective()[0] - hy_bias[num1++];
				}
				else { 
					m_function[num2++]->evaluate_(s, caller::Problem, false, false); 
					fit[i] = s.get_objective()[0];
				}

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
			obj[0] += 1300;
		}
		bool F13_global_composition5::load_hybrid_translation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_HyShift_");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			for (auto &i : m_hybrid)
				i->hybrid_translation().resize(m_variable_size);
			std::ifstream in(s);
			if (in.fail()) {
				set_translation();
				std::ofstream out(s);
				for (size_t i = 0; i < m_num_hybrid; ++i)
					for (size_t j = 0; j < m_variable_size; ++j)
						out << m_hybrid[i]->hybrid_translation()[j] << " ";

				out.close();
			}
			else {
				for (size_t i = 0; i < m_num_hybrid; ++i)
					for (size_t j = 0; j < m_variable_size; ++j)
						in >> m_hybrid[i]->hybrid_translation()[j];
			}
			in.close();
			return true;
		}

		void F13_global_composition5::set_hybrid_translation() {
			for (int i = 0; i < m_num_hybrid; i++)
				for (int j = 0; j < m_variable_size; j++)
					m_hybrid[i]->hybrid_translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - global::ms_global->m_uniform[caller::Problem]->next());
		}
		void F13_global_composition5::set_weight(std::vector<double>& weight, const std::vector<real>&x) {
			size_t num1 = 0;
			size_t num2 = 0;
			for (size_t i = 0; i < m_num_function + m_num_hybrid; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					//weight[i] += pow(x[j] - m_function[i]->translation()[j], 2);
					if(i==0||i==2) weight[i] += pow(x[j] - m_hybrid[num1]->get_optima().variable(0)[j], 2);
					else weight[i] += pow(x[j] - m_function[num2]->get_optima().variable(0)[j], 2);
				}
				if (i == 0 || i == 2) ++num1;
				else ++num2;
				if (fabs(weight[i])>1e-6) weight[i] = exp(-weight[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i])) / (double)(pow(weight[i], 0.5));
				else {
					for (auto &m : weight) {
						m = 0;
					}
					weight[i] = 1;
					break;
				}
			}
		}
		hybrid* F13_global_composition5::get_hybrid(size_t num) {
			return m_hybrid[num];
		}
	}
}