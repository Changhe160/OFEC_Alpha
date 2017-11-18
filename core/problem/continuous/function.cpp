#include "function.h"

namespace OFEC {

	function::function(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj),continuous(name, size_var, size_obj){
		
	}

	void function::set_bias(double val) {
		m_bias = val;
	}

	void function::set_scale(double val) {
		m_scale = val;
		m_scale_flag = true;
	}

	void function::set_rotation_flag(bool flag) {
		m_rotation_flag = flag;
	}

	void function::set_tranlation_flag(bool flag) {
		m_translation_flag = flag;
	}

	void function::set_scale_flag(bool flag) {
		m_scale_flag = flag;
	}

	real function::translation(size_t i) const {
		return m_translation[i];
	}

	std::vector<real>&  function::translation() {
		return m_translation;
	}

	matrix& function::rotation() {
		return m_rotation;
	}

	double function::condition_number() {
		return m_condition_number;
	}
	real function::bias() {
		return m_bias;
	}
	void function::set_condition_number(double c) {
		m_condition_number = c;
	}

	void function::clear() {
		m_translation.clear();
	}

	function& function::operator =(const function & rhs) {
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
		m_rotation = rhs.m_rotation;

		m_original_optima = rhs.m_original_optima;
		return *this;
	}

	function& function::operator =(function && rhs) {
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
		m_rotation = std::move(rhs.m_rotation);

		m_original_optima = std::move(rhs.m_original_optima);
		return *this;
	}

	optima<variable<real>, real>& function::get_original_optima() {
		return m_original_optima;
	}

	void function::translate_zero() {
		for (auto &i : m_translation)
			i = 0;
	}

	bool function::load_translation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_variable_size << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_Shift_");
		s.insert(0, path);    // data path
		s.insert(0, global::ms_arg[param_workingDir]);

		load_translation_(s);

		return true;
	}

	void function::load_translation_(const std::string &path) {
		m_translation.resize(m_variable_size);
		std::ifstream in(path);
		if (in.fail()) {
			if (m_original_optima.variable_given())
				set_translation(m_original_optima.variable(0).data());
			else {
				std::vector<real> zero(m_variable_size, 0);
				set_translation(zero.data());
			}
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

	void function::set_translation(const real *opt_var) {
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

	bool function::load_rotation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_variable_size << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_RotM_");

		s.insert(0, path);// data path
		s.insert(0, global::ms_arg[param_workingDir]);

		load_rotation_(s);

		return true;
	}

	void function::load_rotation_(const std::string &path) {
		m_rotation.resize(m_variable_size, m_variable_size);
		std::ifstream in;
		in.open(path);
		if (in.fail()) {
			set_rotation();
			std::ofstream out(path);
			m_rotation.print(out);
			out.close();
		}
		else {
			m_rotation.load(in);
		}
		in.close();
		m_rotation_flag = true;
	}

	void function::set_rotation() {
		m_rotation.generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), m_condition_number);
	}

	void function::translate(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}
	}
	void function::translate_origin(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] += m_original_optima.variable(0)[i];
		}
	}
	void function::rotate(real *x) {
		double *x_ = new double[m_variable_size];
		std::copy(x, x + m_variable_size, x_);

		for (size_t i = 0; i<m_variable_size; ++i) {
			x[i] = 0;

			for (size_t j = 0; j < m_variable_size; ++j) {
				x[i] += m_rotation[j][i] * x_[j];
			}
		}

		delete[] x_;
		x_ = 0;
	}
	void function::scale(real *x) {
		for (size_t i = 0; i<m_variable_size; ++i)
			x[i] /= m_scale;
	}


	void function::irregularize(real *x) {
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

	void function::asyemmetricalize(real *x, double belta) {
		// this method from BBOB
		if (m_variable_size == 1) return;
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				x[i] = pow(x[i], 1 + belta*i*sqrt(x[i]) / (m_variable_size - 1));
			}
		}
	}

	void function::resize_translation(size_t n) {
		m_translation.resize(n);
	}
	void function::resize_rotation(size_t n) {
		m_rotation.resize(n, n);
	}

	void function::set_original_global_opt(real *opt) {
		if (m_objective_size > 1) throw myexcept("function::set_original_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		if (opt == 0)		for (auto&i : temp_var) i = 0.;
		else	for (int i = 0; i < m_variable_size; i++)  temp_var[i] = opt[i];
		m_original_optima.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_original_optima.variable(0), std::move(temp_obj));

		evaluate_(temp, caller::Problem, false, false);
		m_original_optima.append(std::move(temp.get_objective()));
	}
	void function::set_global_opt(real *tran) {
		if (m_objective_size > 1) throw myexcept("function::set_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		for (int i = 0; i < m_variable_size; ++i) {
			if( tran != 0 ) temp_var[i] = tran[i] + m_original_optima.variable(0)[i];
			else temp_var[i] = m_original_optima.variable(0)[i];
		}
		m_optima.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_optima.variable(0), std::move(temp_obj));

		evaluate_(temp, caller::Problem, false, false);
		m_optima.append(std::move(temp.get_objective()));
	}
	real function::scale() {
		return m_scale;
	}
}