#include "continuous.h"

namespace OFEC {
	violation_type continuous::check_boundary_violation(const base &s) const {

		const variable<real>& x= dynamic_cast<const solution<variable<real>, real>&>(s).get_variable();

		for (int i = 0; i < m_variable_size; ++i) {
			if (m_domain[i].limited) {
				if (x[i]<m_domain[i].limit.first || x[i]>m_domain[i].limit.second) return violation_type::Boundary;
			}
		}

		return violation_type::None;
	}

	void continuous::initialize_solution(base &s) const {
		 variable<real>& x = dynamic_cast<solution<variable<real>, real>&>(s).get_variable();

		for (int i = 0; i < m_variable_size; ++i) {
			if (m_domain[i].limited) {
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(m_domain[i].limit.first, m_domain[i].limit.second);
			}
		}

	}

	void continuous::resize_variable(size_t n) {
		problem::resize_variable(n);

		m_domain.resize(n);
		m_optima.resize_variable(n);
	}
	void continuous::resize_objective(size_t n) {
		m_optima.resize_objective(n);
	}

	void continuous::copy(const problem * rhs) {
		problem::copy(rhs);

		auto p= dynamic_cast<const continuous*>(rhs);
		m_variable_accuracy = p->m_variable_accuracy;

		size_t d = rhs->variable_size() < m_variable_size ? rhs->variable_size(): m_variable_size;
		for (auto i = 0; i < d; ++i) 
			m_domain[i] = p->m_domain[i];
		
		if (m_optima.variable_given()) {

		}

		if (m_optima.objective_given()) {

		}

	}

	bool continuous::is_optimal_given() {
		return m_optima.objective_given() || m_optima.variable_given();
	}
}