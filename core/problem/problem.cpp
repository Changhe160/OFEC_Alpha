#include "problem.h"
#include "../../utility/factory.h"
#include <sstream>
namespace OFEC {

	thread_local std::map<int, std::pair<std::unique_ptr<solution_base>, std::unique_ptr<solution_base>>> problem::ms_minmax_objective; // the best and worst so far solutions of each objective 

	problem::problem(const std::string &name, size_t size_var, size_t size_obj) :m_name(name), m_variable_size(size_var),
		m_objective_size(size_obj), m_opt_mode(size_obj, optimization_mode::Minimization) {
		if(!m_name.empty())
			m_tag = factory<problem>::get().at(m_name).second;
	}

	void problem::resize_objective(size_t n) {
		m_objective_size = n;
		m_opt_mode.resize(n);
	}

	void problem::copy(const problem & rhs) {
		m_name = rhs.m_name;
		m_effective_eval = rhs.m_effective_eval;
		m_total_eval = rhs.m_total_eval;
		//m_objective_size = rhs.m_objective_size;
		//m_variable_size = rhs.m_variable_size;
		m_opt_mode = rhs.m_opt_mode;
		m_objective_accuracy = rhs.m_objective_accuracy;
		m_tag = rhs.m_tag;
		m_solved = rhs.m_solved;
		m_parameters = rhs.m_parameters;

		m_eval_monitor = rhs.m_eval_monitor;
		m_constraint_type = rhs.m_constraint_type;
	}
	void problem::resize_variable(size_t n) {
		m_variable_size = n;
	}

	void problem::set_eval_monitor_flag(bool flag) {
		m_eval_monitor = flag;
	}

	void problem::update_parameters() {
		m_parameters["name"] = m_name;
		m_parameters["scale"] = m_variable_size;
		for (decltype(m_opt_mode.size()) i = 0; i < m_opt_mode.size();++i) {
			std::stringstream str("obj");
			std::string mode[] = { "Minimization", "Maximization" };
			str << i + 1;
			m_parameters[str.str()] = mode[(int)m_opt_mode[i]];
		}
		m_parameters["evals"] = m_effective_eval;

    }

    void problem::reset_alg_records() {
        m_effective_eval = 0;
        m_solved = false;
        m_eval_monitor = false;
    }
}
