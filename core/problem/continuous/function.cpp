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
	
	matrix& function::rotation(size_t i) {
		return m_rotation[i];
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

		transform(x_);
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
}