#include "problem.h"
#include "../../utility/factory.h"

namespace OFEC {

#ifdef OFEC_CONSOLE
	thread_local std::map<int, std::pair<std::unique_ptr<base>, std::unique_ptr<base>>> problem::ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif
#ifdef OFEC_DEMON
	std::map<int, pair<unique_ptr<base>, unique_ptr<base>>> problem::ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif	

	void problem::resize_objective(size_t n) {
		m_objective_size = n;
		m_opt_mode.resize(n);
	}

	void problem::copy(const problem * rhs) {

	}
	void problem::resize_variable(size_t n) {
		m_variable_size = n;
	}

	problem& problem::operator=(const problem& rhs) {
		if (this == &rhs) return *this;

		m_name = rhs.m_name;
		m_effective_eval = rhs.m_effective_eval;
		m_total_eval = rhs.m_total_eval;
		m_objective_size = rhs.m_objective_size;
		m_variable_size = rhs.m_variable_size;
		m_opt_mode = rhs.m_opt_mode;
		m_objective_accuracy = rhs.m_objective_accuracy;
		m_tag = rhs.m_tag;
		m_solved = rhs.m_solved;
		m_paramters.str(rhs.m_paramters.str());

		return *this;
	}

	problem& problem::operator=(problem&& rhs) {
		if (this == &rhs) return *this;

		m_name = std::move(rhs.m_name);
		m_effective_eval = rhs.m_effective_eval;
		m_total_eval = rhs.m_total_eval;
		m_objective_size = rhs.m_objective_size;
		m_variable_size = rhs.m_variable_size;
		m_opt_mode = std::move(rhs.m_opt_mode);
		m_objective_accuracy = rhs.m_objective_accuracy;
		m_tag = std::move(rhs.m_tag);
		m_solved = rhs.m_solved;
		m_paramters = std::move(rhs.m_paramters);
		return *this;
	}

}