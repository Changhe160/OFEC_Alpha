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
#define CONTINUOUS_CAST (dynamic_cast<continuous*>(global::ms_global->m_problem.get()))

	class continuous : virtual public problem {
	public:
		continuous(const std::string &name, size_t size_var, size_t size_obj);

		violation_type check_boundary_violation(const solution_base &s) const override; // boudary check only
		void initialize_solution(solution_base &s) const override;

		const std::pair<real, real>& range(size_t i) const;
		void set_range(real l, real u);
		void set_range(const std::vector<std::pair<real, real>>& r);
		void set_init_range(real l, real u);
		void set_init_range(const std::vector<std::pair<real, real>>& r);

		optima<variable_vector<real>, real>& get_optima();
		std::vector<solution<variable_vector<real>, real>>& get_optima_found();
		const domain<real>& range() const;
		const domain<real>& init_range() const;

		bool same(const solution_base &s1, const solution_base &s2) const override;
		real variable_distance(const solution_base &s1, const solution_base &s2) const override;
		real variable_distance(const variable_base &s1, const variable_base &s2) const override;

		bool is_optima_given() override;
        evaluation_tag evaluate_(solution_base &s, caller call, bool effective, bool initialized) final;

		bool objective_monitor() const;
		bool variable_monitor() const;
		size_t num_optima_found() const;
		void set_variable_monitor_flag(bool flag);
		void set_objective_monitor_flag(bool flag);

		const std::vector<std::vector<size_t>>& variable_partition()const;

	protected:
		void copy(const problem &) override;
		void resize_variable(size_t n) override;
		void resize_objective(size_t n) override;

		virtual void evaluate_objective(real *x, std::vector<real>& obj) {}
		virtual void evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con){}
	protected:
		real m_variable_accuracy = 1.0e-6;
		domain<real> m_domain;		// search domain
		domain<real> m_init_domain; // range for intial population
		optima<variable_vector<real>, real> m_optima;
		std::vector<solution<variable_vector<real>, real>> m_optima_found;
		bool m_variable_monitor = false, m_objective_monitor = false;
		std::vector<std::vector<size_t>> m_variable_partition;
	};

}
#endif // !OFEC_CONTINUOUS_H
