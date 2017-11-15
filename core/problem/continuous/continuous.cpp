#include "continuous.h"

namespace OFEC {
	continuous::continuous(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), m_domain(size_var), m_init_domain(size_var)\
	{

	}

	violation_type continuous::check_boundary_violation(const base &s) const {

		const variable<real>& x = dynamic_cast<const solution<variable<real>, real>&>(s).get_variable();

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
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(m_init_domain[i].limit.first, m_init_domain[i].limit.second);
			}
			else {
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(std::numeric_limits<real>::min(),std::numeric_limits<real>::max());
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

		auto p = dynamic_cast<const continuous*>(rhs);
		m_variable_accuracy = p->m_variable_accuracy;

		size_t d = rhs->variable_size() < m_variable_size ? rhs->variable_size() : m_variable_size;
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

	continuous& continuous::operator=(const continuous& rhs) {
		if (this == &rhs) return *this;
		problem::operator=(rhs);
		m_variable_accuracy = rhs.m_variable_accuracy;
		m_domain = rhs.m_domain;
		m_optima = rhs.m_optima;
		m_variable_monitor = rhs.m_variable_monitor;
		m_objective_monitor = rhs.m_objective_monitor;
		return *this;
	}

	continuous& continuous::operator=(continuous&& rhs) {
		if (this == &rhs) return *this;
		problem::operator=(std::move(rhs));
		m_domain = std::move(rhs.m_domain);
		m_optima = std::move(rhs.m_optima);
		m_variable_monitor = rhs.m_variable_monitor;
		m_objective_monitor = rhs.m_objective_monitor;
		return *this;
	}

	const std::pair<real, real>& continuous::range(int i) const {
		return m_domain.range(i).limit;
	}
	void continuous::set_range(real l, real u) {
		for (size_t i = 0; i < m_domain.size(); ++i)
			m_domain.set_range(l, u, i);
	}

	void continuous::set_range(const std::vector<std::pair<real, real>>& r) {
		size_t count = 0;
		for (auto &i : r) {
			m_domain.set_range(i.first, i.second, count++);
		}
	}

	void continuous::set_init_range(real l, real u) {
		for (size_t i = 0; i < m_domain.size(); ++i)
			m_init_domain.set_range(l, u, i);
	}
	void continuous::set_init_range(const std::vector<std::pair<real, real>>& r) {
		size_t count = 0;
		for (auto &i : r) {
			m_init_domain.set_range(i.first, i.second, count++);
		}
	}

	optima<variable<real>, real>& continuous::get_optima() {
		return m_optima;
	}

	double continuous::variable_distance(const base &s1, const base &s2) const {
		const variable<real>& x1 = dynamic_cast<const solution<variable<real>, real>&>(s1).get_variable();
		const variable<real>& x2 = dynamic_cast<const solution<variable<real>, real>&>(s2).get_variable();
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());

	}

	double continuous::variable_distance(const variable_base &s1, const variable_base &s2) const {
		const variable<real>& x1 = dynamic_cast<const variable<real>&>(s1);
		const variable<real>& x2 = dynamic_cast<const variable<real>&>(s2);
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());
	}

	evaluation_tag continuous::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
		variable<real> &x = dynamic_cast< solution<variable<real>, real> &>(s).get_variable();
		auto & obj = dynamic_cast< solution<variable<real>, real> &>(s).get_objective();
		double & cons_value = dynamic_cast<solution<variable<real>, real> &>(s).constraint_value().first;
		std::vector<double> & cons_values = dynamic_cast<solution<variable<real>, real> &>(s).constraint_value().second;

		std::vector<real> x_(x.begin(), x.end()); //for parallel running

		if (is_tag(problem_tag::COP)) evaluate__(x_.data(), obj, cons_value, cons_values);
		else evaluate__(x_.data(), obj);

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