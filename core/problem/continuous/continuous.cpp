#include "continuous.h"
#include "../../../utility/functional.h"

namespace OFEC {
	continuous::continuous(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), m_domain(size_var), m_init_domain(size_var) {}

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
			if (m_init_domain[i].limited)
				x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(m_init_domain[i].limit.first, m_init_domain[i].limit.second);
			else {                           
				if (m_domain[i].limited)     // If m_init_domain is not given, then use problem boundary as initialization range
					x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(m_domain[i].limit.first, m_domain[i].limit.second);
				else                         // Else if the problem function has no boundary 
					x[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(-std::numeric_limits<real>::max(), std::numeric_limits<real>::max());
			}
		}
	}

	void continuous::resize_variable(size_t n) {
		problem::resize_variable(n);
		m_domain.resize(n);
	}
	void continuous::resize_objective(size_t n) {
		m_optima.resize_objective(n);
	}

	void continuous::copy(const problem  &rhs) {
		problem::copy(rhs);

		auto& p =  dynamic_cast<const continuous&>(rhs);
		m_variable_accuracy = p.m_variable_accuracy;

		size_t d = rhs.variable_size() < m_variable_size ? rhs.variable_size() : m_variable_size;
		for (size_t i = 0; i < d; ++i) {
			m_domain[i] = p.m_domain[i];
			m_init_domain[i] = p.m_init_domain[i];
		}
		m_variable_monitor = p.m_variable_monitor;
		m_objective_monitor = p.m_objective_monitor;

	}

	bool continuous::is_optima_given() {
		return m_optima.objective_given() || m_optima.variable_given();
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
	const domain<real>& continuous::range() const {
		return m_domain;
	}

	const domain<real>& continuous::init_range() const {
		return m_init_domain;
	}

	real continuous::variable_distance(const solution_base &s1, const solution_base &s2) const {
		const variable_vector<real>& x1 = dynamic_cast<const solution<variable_vector<real>, real>&>(s1).variable();
		const variable_vector<real>& x2 = dynamic_cast<const solution<variable_vector<real>, real>&>(s2).variable();
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());

	}

	real continuous::variable_distance(const variable_base &s1, const variable_base &s2) const {
		const auto& x1 = dynamic_cast<const variable_vector<real>&>(s1);
		const auto& x2 = dynamic_cast<const variable_vector<real>&>(s2);
		return euclidean_distance(x1.begin(), x1.end(), x2.begin());
	}

	evaluation_tag continuous::evaluate_(solution_base &s, caller call, bool effective, bool initialized) {
		variable_vector<real> &x = dynamic_cast< solution<variable_vector<real>, real> &>(s).variable();
		auto & obj = dynamic_cast< solution<variable_vector<real>, real> &>(s).objective();
		auto & con = dynamic_cast<solution<variable_vector<real>, real> &>(s).constraint_value();

		std::vector<real> x_(x.begin(), x.end()); //for parallel running

		if (con.empty()) evaluate_objective(x_.data(), obj);
		else  evaluate_obj_nd_con(x_.data(), obj, con);

		if (initialized && call == caller::Algorithm) {
			if (effective)		m_effective_eval++;

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
			if (global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
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

	const std::vector<std::vector<size_t>>& continuous::variable_partition() const {
		return m_variable_partition;
	}

	bool continuous::same(const solution_base &s1, const solution_base &s2) const { 
		return dynamic_cast<const solution<variable_vector<real>, real> &>(s1).variable().vect() == dynamic_cast<const solution<variable_vector<real>, real> &>(s2).variable().vect();
	}

}