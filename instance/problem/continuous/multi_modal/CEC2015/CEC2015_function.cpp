#include "CEC2015_function.h"

namespace OFEC {
	namespace CEC2015 {
		CEC2015_function::CEC2015_function(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), 
			function(name, size_var, size_obj)
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			m_opt_mode[0] = optimization_mode::Minimization;
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			set_condition_number(1.0);
		}
		
		bool CEC2015_function::load_translation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << ".txt";
			s = ss.str();
			s.insert(0, m_name + "_Shift");
			s.insert(0, path);    // data path
			s.insert(0, global::ms_arg[param_workingDir]);

			load_translation_(s);

			return true;
		}
		void CEC2015_function::load_translation_(const std::string &path) {
			m_translation.resize(m_variable_size);
			std::ifstream in(path);
			if (in.fail()) {
				
				std::vector<real> temp(m_variable_size, 20);
				set_translation(temp.data());
				
				std::ofstream out(path);
				for (auto &i : m_translation)
					out << i << " ";
				out.close();
			}
			else {
				for (auto &i : m_translation)
					in >> i;
			}
			in.close();
			m_translation_flag = true;
		}

		
		bool CEC2015_function::load_optima(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_variable_size << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_Optima_");

			s.insert(0, path);// data path
			s.insert(0, global::ms_arg[param_workingDir]);

			load_optima_(s);

			return true;
		}

		void CEC2015_function::load_optima_(const std::string &path) {
			std::ifstream in;
			in.open(path);
			if (in.fail()) {
				set_optima();
				std::ofstream out(path);
				for (size_t i = 0; i < m_optima.number_variable();++i)
					for (size_t j = 0; j < m_variable_size; ++j)
						out << m_optima.variable(i)[j] << " ";
				out.close();
			}
			else {
				while(!(in.eof())) {
					variable<real> temp_var(m_variable_size);
					for (size_t j = 0; j < m_variable_size; ++j)
						in >> temp_var[j];
					m_optima.append(std::move(temp_var));
				}
			}
			in.close();
		}

		void CEC2015_function::set_optima() {
			//  to do ..
			throw myexcept("Waiting to do !");
		}

		void CEC2015_function::evaluate_optima(){
			for (size_t i = 0; i < m_optima.number_variable(); ++i) {
				variable<real> temp_var(m_optima.variable(i));
				objective<real> temp_obj(m_objective_size);
				solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
				evaluate_(x, caller::Problem, false, false);
				m_optima.append(x.get_objective());
			}
			
		}
		void CEC2015_function::rotate(real *x) {
			double *x_ = new double[m_variable_size];
			std::copy(x, x + m_variable_size, x_);

			for (size_t i = 0; i<m_variable_size; ++i) {
				x[i] = 0;

				for (size_t j = 0; j < m_variable_size; ++j) {
					x[i] += m_rotation[i][j] * x_[j];
				}
			}

			delete[] x_;
			x_ = 0;
		}
	}
}