#include "F14_composition2015_C6.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/modified_schwefel.h"

namespace OFEC {
	namespace CEC2015 {
		F14_composition2015_C6::F14_composition2015_C6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition_2015((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 10;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			initialize();
		}
		F14_composition2015_C6::F14_composition2015_C6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			m_num_function = 10;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			initialize();
		}

		void F14_composition2015_C6::set_function() {
			basic_func f(2);
			f[0] = &create_function<rastrigin>;
			f[1] = &create_function<modified_schwefel>;

			for (size_t i = 0; i < m_num_function; ++i) {
				if(i/2 == 0) m_function[i] = dynamic_cast<function*>(f[0]("", m_variable_size, m_objective_size));
				else  m_function[i] = dynamic_cast<function*>(f[1]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			for (auto &i : m_function)
				i->set_condition_number(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 10;
			m_converge_severity[2] = 20;
			m_converge_severity[3] = 20;
			m_converge_severity[4] = 30;
			m_converge_severity[5] = 30;
			m_converge_severity[6] = 40;
			m_converge_severity[7] = 40;
			m_converge_severity[8] = 50;
			m_converge_severity[9] = 50;

			m_height[0] = 10;
			m_height[1] = 1;
			m_height[2] = 10;
			m_height[3] = 1;
			m_height[4] = 10;
			m_height[5] = 1;
			m_height[6] = 10;
			m_height[7] = 1;
			m_height[8] = 10;
			m_height[9] = 1;

			for (size_t i = 0; i < m_num_function; ++i) {
				if (i / 2 == 0) m_function[i]->set_scale(100. / 5.12);
				else m_function[i]->set_scale(1. / 10.);
			}
			real temp = 0;
			for (auto &i : m_f_bias) {
				i = temp;
				temp += 20;
			}

		}

		void F14_composition2015_C6::initialize() {
			set_function();
			load_translation("instance/problem/continuous/multi_modal/CEC2015/data/");
			load_rotation("instance/problem/continuous/multi_modal/CEC2015/data/");
			for (auto &i : m_function)
				i->set_rotation_flag(false);
			for (auto &i : m_function)
				i->set_scale_flag(false);
			// 10 or 20 or 30 Dim : 1 gopt and 19 lopt 
			m_optima.set_number_variable(20);
			if (m_variable_size == 10) m_pre_opt_distance = 113;
			else if(m_variable_size == 20) m_pre_opt_distance = 183;
			else if(m_variable_size == 30) m_pre_opt_distance = 285;
			
			add_tag(problem_tag::MMP);
		}
		void F14_composition2015_C6::evaluate__(real *x, std::vector<real>& obj) {
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
				for (size_t j = 0; j < m_variable_size; ++j)
					s.get_variable()[j] -= m_function[i]->translation()[j];
				rotate(i, s.get_variable().data());
				scale(i, s.get_variable().data());
				
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
			obj[0] += 1400;
		}

		bool F14_composition2015_C6::load_translation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << ".txt";
			s = ss.str();
			s.insert(0, m_name + "_Shift");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			for (auto &i : m_function)
				i->translation().resize(m_variable_size);
			std::ifstream in(s);
			if (in.fail()) {
				throw myexcept("open translation file failed !");
				/*set_translation();
				std::ofstream out(s);
				for (size_t i = 0; i < m_num_function; ++i) {
				for (size_t j = 0; j < m_variable_size; ++j)
				out << m_function[i]->translation()[j] << " ";
				}

				out.close();*/
			}
			else {
				for (size_t i = 0; i < m_num_function; ++i) {
					size_t count = 0;
					real temp;
					while (count < 100) {
						++count;
						if (count <= m_variable_size)
							in >> m_function[i]->translation()[count - 1];
						else {
							in >> temp;
						}
					}
				}
			}
			in.close();

			return true;
		}
		void F14_composition2015_C6::set_translation() {
			for (int i = 0; i < m_num_function; i++)
				for (int j = 0; j < m_variable_size; j++)
					m_function[i]->translation()[j] = (global::ms_global->m_uniform[caller::Problem]->next() - 0.5) * 2 * 80.;;
		}

		void F14_composition2015_C6::rotate(size_t num, real *x) {
			double *x_ = new double[m_variable_size];
			std::copy(x, x + m_variable_size, x_);

			for (size_t i = 0; i<m_variable_size; ++i) {
				x[i] = 0;

				for (size_t j = 0; j < m_variable_size; ++j) {
					x[i] += m_function[num]->rotation()[i][j] * x_[j];
				}
			}

			delete[] x_;
			x_ = 0;
		}

		void F14_composition2015_C6::scale(size_t num, real *x) {
			for (size_t i = 0; i<m_variable_size; ++i)
				x[i] /= m_function[num]->scale();
		}
		real F14_composition2015_C6::pre_opt_distance() {
			return m_pre_opt_distance;
		}
		void F14_composition2015_C6::set_weight(std::vector<double>& weight, const std::vector<real>&x) {

			for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
				weight[i] = 0;
				for (size_t j = 0; j < m_variable_size; ++j) {
					//weight[i] += pow(x[j] - m_function[i]->translation()[j], 2);
					weight[i] += pow(x[j] - m_function[i]->get_optima().variable(0)[j], 2);
				}

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
	}
}