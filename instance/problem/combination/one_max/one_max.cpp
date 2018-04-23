#include "one_max.h"

namespace OFEC {
	one_max::one_max(param_map & v) : problem(v.at("proName"), v.at("numDim"), 1)
	{
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Maximization;
		m_optima.append(variable<int>(std::vector<int>(m_variable_size, 1)));
		m_optima.append(m_variable_size);
	}
	one_max::one_max(const std::string & name, int size_var) : problem(name, size_var, 1)
	{
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Maximization;
		m_optima.append(variable<int>(std::vector<int>(m_variable_size, 1)));
		m_optima.append(m_variable_size);
	}
	evaluation_tag one_max::evaluate_(base & s, caller call, bool effective_fes, bool constructed)
	{
		variable<int> &x = dynamic_cast< solution<variable<int>, real> &>(s).get_variable();
		std::vector<double> &obj = dynamic_cast<solution<variable<int>, real> &>(s).get_objective();

		for (int i = 0; i < m_objective_size; i++)
			obj[i] = 0;

		for (int n = 0; n < m_objective_size; n++)
		{
			for (size_t i = 0; i < m_variable_size; i++)
			{
				obj[n] += x[i];
			}
		}

		if (constructed) {
			if (effective_fes)		m_effective_eval++;

			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;
		}
		return evaluation_tag::Normal;
	}
	bool one_max::is_valid(const base & s)
	{
		if (!m_if_valid_check) return true;
		const variable<int> &x = dynamic_cast<const solution<variable<int>, real> &>(s).get_variable();
		for (int i = 0; i < m_variable_size; i++) {
			if (x[i] != 0 && x[i] != 1)
				return false;
		}
		return true;
	}
	void one_max::initialize_solution(base & s) const
	{
		variable<int> &x = dynamic_cast< solution<variable<int>, real> &>(s).get_variable();
		for (size_t i = 0; i < m_variable_size; i++)
			if (global::ms_global->m_uniform[caller::Problem]->next() < 0.5)
				x[i] = 0;
			else x[i] = 1;
	}
	bool one_max::same(const base & s1, const base & s2) const
	{
		const variable<int> &x1 = dynamic_cast<const solution<variable<int>, real> &>(s1).get_variable();
		const variable<int> &x2 = dynamic_cast<const solution<variable<int>, real> &>(s2).get_variable();
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	double one_max::variable_distance(const base & s1, const base & s2) const
	{
		const variable<int> &x1 = dynamic_cast<const solution<variable<int>, real> &>(s1).get_variable();
		const variable<int> &x2 = dynamic_cast<const solution<variable<int>, real> &>(s2).get_variable();
		double dis = 0;
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	double one_max::variable_distance(const variable_base & s1, const variable_base & s2) const
	{
		const variable<int> &x1 = dynamic_cast<const variable<int>&>(s1);
		const variable<int> &x2 = dynamic_cast<const variable<int>&>(s2);
		double dis = 0;
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}
}