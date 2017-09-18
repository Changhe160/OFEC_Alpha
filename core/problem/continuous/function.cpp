#include "function.h"

namespace OFEC {
	
	function::function(const std::string &name, size_t size_var, size_t size_obj) :continuous(name, size_var, size_obj) {

	}
	
	void function::set_bias(double val) {
		m_bias = val;
	}
	
	void function::set_scale(double val) {
		m_scale = val;
	}
	
	void function::set_rotation_flag(bool flag) {
		m_rotation_flag = flag;
	}
	
	void function::set_tranlation_flag(bool flag) {
		m_translation_flag = flag;
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
	
	void function::set_condition_number(double c) {
		m_condition_number = c;
	}
	
	evaluation_tag function::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
		variable<real> &x = dynamic_cast< solution<variable<real>, real> &>(s).get_variable();
		auto & obj = dynamic_cast< solution<variable<real>, real> &>(s).get_objective();

		double *x_ = new double[m_variable_size]; //for parallel running
		std::copy(x.begin(), x.end(), x_);

		evaluate__(x_, obj);
		delete[] x_;
		x_ = 0;
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

	void function::translate(real *x) {
		for (int i = 0; i<m_variable_size; i++) {
			x[i] -= m_optima.variable(0)[i];
		}
	}
	void function::rotate(real *x) {
		double *x_ = new double[m_variable_size];
		std::copy(x, x + m_variable_size, x_);

		for (int i = 0; i<m_variable_size; i++) {
			x[i] = 0;

			for (int j = 0; j < m_variable_size; j++) {
				x[i] += m_rotation[j][i] * x_[j];
			}
		}

		delete[] x_;
		x_ = 0;
	}
	void function::scale(real *x) {
		for (int i = 0; i<m_variable_size; i++)     
			x[i] /= m_scale;
	}

	void function::irregularize(real *x) {
		double c1, c2, x_;
		for (int i = 0; i < m_variable_size; ++i) {
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
		if (m_variable_size == 1) return;
		for (int i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				x[i] = pow(x[i], 1 + belta*i*sqrt(x[i]) / (m_variable_size - 1));
			}
		}
	}

	void function::resize_translation(size_t n) {
		m_translation.resize(n);
	}
	void function::resize_rotation(size_t n) {
		m_rotation.resize(n,n);
	}
	void function::set_rotation() {

	}
}