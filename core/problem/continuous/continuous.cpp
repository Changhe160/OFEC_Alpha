#include "continuous.h"

namespace OFEC {
	continuous::continuous(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), m_domain(size_var), m_init_domain(size_var)\
	{
	
	}

	violation_type continuous::check_boundary_violation(const solution_base &s) const {

		const variable_vector<real>& x = dynamic_cast<const solution<variable_vector<real>, real>&>(s).variable();

		for (int i = 0; i < m_variable_size; ++i) {
			if (m_domain[i].limited) {
				if (x[i]<m_domain[i].limit.first || x[i]>m_domain[i].limit.second) return violation_type::Boundary;
			}
		}

		return violation_type::None;
	}

	void continuous::initialize_solution(solution_base &s) const {
		variable_vector<real>& x = dynamic_cast<solution<variable_vector<real>, real>&>(s).variable();

		for (int i = 0; i < m_variable_size; ++i) {
			if (m_domain[i].limited) {
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(m_init_domain[i].limit.first, m_init_domain[i].limit.second);
			}
			else {
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(std::numeric_limits<real>::min(), std::numeric_limits<real>::max());
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

	const std::pair<real, real>& continuous::range(size_t i) const {
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

	optima<variable_vector<real>, real>& continuous::get_optima() {
		return m_optima;
	}
	std::vector<solution<variable_vector<real>, real>>& continuous::get_optima_found() {
		return m_optima_found;
	}
	domain<real>& continuous::range() {
		return m_domain;
	}

	double continuous::variable_distance(const solution_base &s1, const solution_base &s2) const {
		const variable_vector<real>& x1 = dynamic_cast<const solution<variable_vector<real>, real>&>(s1).variable();
		const variable_vector<real>& x2 = dynamic_cast<const solution<variable_vector<real>, real>&>(s2).variable();
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());

	}

	double continuous::variable_distance(const variable_base &s1, const variable_base &s2) const {
		const variable_vector<real>& x1 = dynamic_cast<const variable_vector<real>&>(s1);
		const variable_vector<real>& x2 = dynamic_cast<const variable_vector<real>&>(s2);
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());
	}

	evaluation_tag continuous::evaluate_(solution_base &s, caller call, bool effective_fes, bool constructed) {
		variable_vector<real> &x = dynamic_cast< solution<variable_vector<real>, real> &>(s).variable();
		auto & obj = dynamic_cast< solution<variable_vector<real>, real> &>(s).objective();
		double & cons_value = dynamic_cast<solution<variable_vector<real>, real> &>(s).constraint_value().first;
		std::vector<double> & cons_values = dynamic_cast<solution<variable_vector<real>, real> &>(s).constraint_value().second;

		std::vector<real> x_(x.begin(), x.end()); //for parallel running

		if (has_tag(problem_tag::COP)) evaluate__(x_.data(), obj, cons_value, cons_values);
		else evaluate__(x_.data(), obj);

		if (constructed) {
			if (effective_fes)		m_effective_eval++;

			if (m_variable_monitor) {
				m_optima.is_optimal_variable(dynamic_cast<solution<variable_vector<real>, real> &>(s), m_optima_found, m_variable_accuracy);
				if (m_optima.is_variable_found())
					m_solved = true;
			}
			if (m_objective_monitor) {
				m_optima.is_optimal_objective(dynamic_cast<solution<variable_vector<real>, real> &>(s), m_optima_found, m_objective_accuracy, m_variable_accuracy);
				if (m_optima.is_objective_found())
					m_solved = true;
			}
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;
		}
		return evaluation_tag::Normal;
	}
	
	bool continuous::objective_monitor() const {
		return m_objective_monitor;
	}
	bool continuous::variable_monitor() const {
		return m_variable_monitor;
	}

	size_t continuous::num_optima_found() const {
		if (m_variable_monitor)
			return m_optima.num_variable_found();
		else if (m_objective_monitor)
			return m_optima.num_objective_found();
	}

	void continuous::set_variable_monitor_flag(bool flag) {
		m_variable_monitor = flag;
	}
	void continuous::set_objective_monitor_flag(bool flag) {
		m_objective_monitor = flag;
	}

}