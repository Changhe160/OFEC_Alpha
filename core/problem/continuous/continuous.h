/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class continuous defines continous optimization problems
*
*********************************************************************************/
#ifndef OFEC_CONTINUOUS_H
#define OFEC_CONTINUOUS_H

#include "../problem.h"
#include "../optima.h"
#include "../domain.h"

namespace OFEC {
#define CONTINOUS_CAST dynamic_cast<continuous*>( global::ms_global->m_problem.get())

	class continuous : virtual public problem {
	public:
		continuous(const std::string &name, size_t size_var, size_t size_obj);

		violation_type check_boundary_violation(const base &s) const; // boudary check only
		void initialize_solution(base &s) const;

		const std::pair<real, real>& range(int i) const;
		void set_range(real l, real u);
		void set_range(const std::vector<std::pair<real, real>>& r);
		void set_init_range(real l, real u);
		void set_init_range(const std::vector<std::pair<real, real>>& r);

		optima<variable<real>, real>& get_optima();

		virtual bool same(const base &s1, const base &s2) const { return false; }
		double variable_distance(const base &s1, const base &s2) const;
		double variable_distance(const variable_base &s1, const variable_base &s2) const;

		//virtual void constraint_value(const base &, std::pair<double, vector<double>>&) {}
		bool is_optimal_given();
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
	protected:
		continuous& operator=(const continuous& rhs);
		continuous& operator=(continuous&& rhs);

		void copy(const problem * rhs);
		void resize_variable(size_t n);
		void resize_objective(size_t n);

		virtual void evaluate__(real *x, std::vector<real>& obj) {}
		virtual void evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values) {}
	protected:
		double m_variable_accuracy = 1.0e-6;
		domain<real> m_domain;		// search domain
		domain<real> m_init_domain; // range for intial population
		optima<variable<real>, real> m_optima;
		bool m_variable_monitor = false, m_objective_monitor = false;
	};

}
#endif // !OFEC_CONTINUOUS_H