#include "constraint.h"

namespace OFEC {

	constraint::constraint(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), continuous(name, size_var, size_obj) {
		global::ms_arg[param_workingDir] = "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/";
	}

	void constraint::set_bias(double val) {
		m_bias = val;
	}

	void constraint::set_scale(double val) {
		m_scale = val;
		m_scale_flag = true;
	}

	void constraint::set_rotation_flag(bool flag) {
		m_rotation_flag = flag;
	}

	void constraint::set_tranlation_flag(bool flag) {
		m_translation_flag = flag;
	}

	real constraint::translation(size_t i) const {
		return m_translation[i];
	}

	std::vector<real>&  constraint::translation() {
		return m_translation;
	}

	std::vector<matrix>& constraint::rotation() {
		return m_rotation;
	}

	double constraint::condition_number() {
		return m_condition_number;
	}

	void constraint::set_condition_number(double c) {
		m_condition_number = c;
	}

	void constraint::clear() {
		m_translation.clear();
	}

	constraint& constraint::operator =(const constraint & rhs) {
		if (this == &rhs) return *this;
		continuous::operator=(rhs);

		m_scale_flag = rhs.m_scale_flag;
		m_rotation_flag = rhs.m_rotation_flag;
		m_translation_flag = rhs.m_translation_flag;
		m_noise_flag = rhs.m_noise_flag;

		m_scale = rhs.m_scale;
		m_bias = rhs.m_bias;
		m_condition_number = rhs.m_condition_number;

		m_translation = rhs.m_translation;
		for (size_t i = 0; i < rhs.m_rotation.size();++i)
			m_rotation[i] = rhs.m_rotation[i];

		m_original_optima = rhs.m_original_optima;
		return *this;
	}

	constraint& constraint::operator =(constraint && rhs) {
		if (this == &rhs) return *this;
		continuous::operator=(std::move(rhs));

		m_scale_flag = rhs.m_scale_flag;
		m_rotation_flag = rhs.m_rotation_flag;
		m_translation_flag = rhs.m_translation_flag;
		m_noise_flag = rhs.m_noise_flag;

		m_scale = rhs.m_scale;
		m_bias = rhs.m_bias;
		m_condition_number = rhs.m_condition_number;

		m_translation = std::move(rhs.m_translation);
		
		for (size_t i = 0; i < rhs.m_rotation.size(); ++i)
			m_rotation[i] = std::move(rhs.m_rotation[i]);

		m_original_optima = std::move(rhs.m_original_optima);
		return *this;
	}


	evaluation_tag constraint::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
		variable<real> &x = dynamic_cast< solution<variable<real>, real> &>(s).get_variable();
		auto & obj = dynamic_cast< solution<variable<real>, real> &>(s).get_objective();
		double & cons = dynamic_cast<solution<variable<real>, real> &>(s).constraint_value();
		vector<real> x_(x.begin(), x.end()); //for parallel running

		evaluate__(x_.data(), obj, cons);

		if (constructed) {
			if (effective_fes)		m_effective_eval++;

			if (m_variable_monitor) {
				m_optima.is_optimal_variable(dynamic_cast<solution<variable<real>, real> &>(s), m_variable_accuracy);
				if (m_optima.is_variable_found())
					m_solved = true;
			}
			if (m_objective_monitor) {
				m_optima.is_optimal_objective(obj, m_objective_accuracy);
				if (m_optima.is_objective_found())
					m_solved = true;
			}
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;

			//if (mode == Program_Algorithm&&Global::msp_global->mp_problem && !Global::msp_global->mp_problem->isProTag(MOP)) m_globalOpt.isFound(s, m_disAccuracy, m_accuracy);
			//if (Global::msp_global != nullptr&&Global::msp_global->mp_algorithm != nullptr&&Global::msp_global->mp_algorithm->ifTerminating()) { return Return_Terminate; }
		}
		return evaluation_tag::Normal;
	}

	optima<variable<real>, real>& constraint::get_original_optima() {
		return m_original_optima;
	}

	void constraint::translate_zero() {
		for (auto &i : m_translation)
			i = 0;
	}

	bool constraint::load_translation(const string &path) {
		string s;
		stringstream ss;
		ss << m_variable_size << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_Opt_");
		s.insert(0, path);    // data path
		s.insert(0, global::ms_arg[param_workingDir]);

		return load_translation_(s);
	}

	bool constraint::load_translation_(const string &path) {
		ifstream in(path);
		if (in.fail()) {
			return false;
		}
		else {
			for (auto &i : m_translation)
				in >> i;
		}
		in.close();
		m_translation_flag = true;
		return true;
	}

	void constraint::set_translation(const std::vector<real>& opt_var) {
		// Initial the location of shifted global optimum

		for (size_t j = 0; j<m_variable_size; ++j) {
			real x, rl, ru, range;
			x = opt_var[j];
			ru = m_domain[j].limit.second - x;
			rl = x - m_domain[j].limit.first;
			range = rl<ru ? rl : ru;
			m_translation[j] = (global::ms_global->m_uniform[caller::Problem]->next() - 0.5) * 2 * range;
		}
		m_translation_flag = true;
	}

	bool constraint::load_rotation(const string &path) {
		string s;
		stringstream ss;
		ss << m_variable_size << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_RotM_");

		s.insert(0, path);// data path
		s.insert(0, global::ms_arg[param_workingDir]);

		load_rotation_(s);

		return true;
	}

	void constraint::load_rotation_(const string &path) {
		ifstream in;
		in.open(path);
		if (in.fail()) {
			set_rotation();
			ofstream out(path);
			for (auto &i : m_rotation)
				i.print(out);
			out.close();
		}
		else {
			for (auto &i : m_rotation)
				i.load(in);
		}
		in.close();
		m_rotation_flag = true;
	}

	void constraint::set_rotation() {
		for(auto &i: m_rotation)
			i.generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), m_condition_number);
	}

	void constraint::translate(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}
	}
	void constraint::translate_origin(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] += m_original_optima.variable(0)[i];
		}
	}

	void constraint::translate_back(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] -= m_original_optima.variable(0)[i];
		}
	}
	void constraint::rotate(real *x, size_t num) {
		double *x_ = new double[m_variable_size];
		std::copy(x, x + m_variable_size, x_);

		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] = 0;

			for (size_t j = 0; j < m_variable_size; ++j) {
				x[i] += m_rotation[num-1][j][i] * x_[j];
			}
		}

		delete[] x_;
		x_ = 0;
	}
	void constraint::scale(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i)
			x[i] /= m_scale;
	}


	void constraint::irregularize(real *x) {
		// this method from BBOB
		double c1, c2, x_;
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				c1 = 10;	c2 = 7.9;
			}
			else {
				c1 = 5.5;	c2 = 3.1;
			}
			if (x[i] != 0) {
				x_ = log(fabs(x[i]));
			}
			else x_ = 0;
			x[i] = sign(x[i])*exp(x_ + 0.049*(sin(c1*x_) + sin(c2*x_)));
		}
	}

	void constraint::asyemmetricalize(real *x, double belta) {
		// this method from BBOB
		if (m_variable_size == 1) return;
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				x[i] = pow(x[i], 1 + belta*i*sqrt(x[i]) / (m_variable_size - 1));
			}
		}
	}

	void constraint::resize_translation(size_t n) {
		m_translation.resize(n);
	}
	void constraint::resize_rotation(size_t n) {
		for(auto &i: m_rotation)
			i.resize(n, n);
	}

	void constraint::set_original_global_opt(real *opt) {
		if (m_objective_size > 1) throw myexcept("constraint::set_original_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		if (opt == 0)		for (auto&i : temp_var) i = 0.;
		else	for (int i = 0; i < m_variable_size; i++)  temp_var[i] = opt[i];
		m_original_optima.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_original_optima.variable(0), std::move(temp_obj));

		evaluate_(temp, caller::Problem, false, false);
		m_original_optima.append(std::move(temp.get_objective()));
	}
	void constraint::set_global_opt(real *tran) {
		if (m_objective_size > 1) throw myexcept("constraint::set_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		for (int i = 0; i < m_variable_size; ++i) {
			if (tran != 0) temp_var[i] = tran[i] + m_original_optima.variable(0)[i];
			else temp_var[i] = m_original_optima.variable(0)[i];
		}
		m_optima.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_optima.variable(0), std::move(temp_obj));

		evaluate_(temp, caller::Problem, false, false);
		m_optima.append(std::move(temp.get_objective()));
	}

}