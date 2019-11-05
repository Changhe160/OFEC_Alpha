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
* class solution is designed for any kind of user-defined solution representation. 
* It takes a variable encoding and an objective value type as parameters to generate
* a solution.
*
*********************************************************************************/

#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H
#include <utility>

#include "../measure/measure.h"

namespace  OFEC {

	template<typename VariableEncoding = variable_vector<real>, 
		typename ObjetiveType = real
	>
	class solution:public solution_base{
	public:
		using objective_encoding = ObjetiveType;
		using variable_encoding = VariableEncoding;
		solution() = default;
		virtual ~solution() {};
		template<typename ... Args>
		solution(size_t num_obj, size_t num_con, Args&& ... args) : m_var(std::forward<Args>(args)...), \
		    m_obj(num_obj), m_constraint_value(num_con) {}
		solution(const solution& rhs) = default;
		solution(solution&& rhs) = default;

		solution& operator =(const solution& rhs) = default;
		solution& operator =(solution&& rhs) = default;

		virtual void initialize() {
			global::ms_global->m_problem->initialize_solution(*this);
		}

		template<typename Initializer, typename... Args>
		void initialize(Initializer f, Args&&... args) {
			f(std::forward<Args>(args)...);
		}

		size_t objective_size() const noexcept { return m_obj.size(); }

		virtual void resize_objective(size_t n) { 
			m_obj.resize(n); 
		}

		bool dominate(const std::vector<objective_encoding>& o) const{ //this soluton donimates objective o
			return  dominationship::Dominating == objective_compare(m_obj, o,  global::ms_global->m_problem->opt_mode());
		}

		bool dominate(const solution& s) const {//this solution donimates solution s
			return dominationship::Dominating == objective_compare(m_obj, s.m_obj, global::ms_global->m_problem->opt_mode());
		}

		bool dominate_equal(const std::vector<objective_encoding>& o)const { //this soluton weakly donimates objective o
			 dominationship r = objective_compare(m_obj, o, global::ms_global->m_problem->opt_mode());
			 return r == dominationship::Dominating || r == dominationship::Equal;
		}

		bool dominate_equal(const solution& s)const {//this solution weakly donimates solution s
			dominationship r = objective_compare(m_obj,s.m_obj, global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		bool nondominate(const std::vector<objective_encoding>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj, o, global::ms_global->m_problem->opt_mode());
		}

		bool nondominate(const solution& s)const {//two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj, s.m_obj, global::ms_global->m_problem->opt_mode());
		}
	 
		bool equal(const solution& rhs) const {
			return dominationship::Equal == objective_compare(m_obj, rhs.m_obj, global::ms_global->m_problem->opt_mode());
		}

		bool same(const solution& x)const {//two solutions in decision space
			return global::ms_global->m_problem->same(*this, x);
		}

		evaluation_tag evaluate(bool effective_eval=true, caller call= caller::Algorithm) {
			if (global::ms_global->m_algorithm != nullptr && global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;
			auto tag = global::ms_global->m_problem->evaluate(*this, call, effective_eval);
			if (measure::get_measure() && effective_eval && (tag == evaluation_tag::Terminate ||
				(global::ms_global->m_problem->evaluations() > 0 && global::ms_global->m_problem->evaluations() % global::ms_sample_fre == 0))) {
				global::ms_global->m_algorithm->record();
			}
			return tag;
		}

		real objective_distance(const std::vector<objective_encoding>& rhs) const {
			if (objective_size() == 1) return fabs(m_obj[0] - rhs[0]);

			return euclidean_distance(m_obj.begin(), m_obj.end(), rhs.begin());
		}

		real objective_distance(const solution& rhs) const {
			if (objective_size() == 1) return fabs(m_obj[0] - rhs.m_obj[0]);

			return euclidean_distance(m_obj.begin(), m_obj.end(), rhs.m_obj.begin());
		}


		real variable_distance(const solution& rhs) const {			
			return global::ms_global->m_problem->variable_distance(*this, rhs);
		}

		real variable_distance(const variable_encoding& x) const {
			return global::ms_global->m_problem->variable_distance(m_var, x);
		}

		const variable_encoding& variable()const noexcept {
			return m_var;
		}

		variable_encoding& variable()noexcept {
			return m_var;
		}

		std::vector<objective_encoding>& objective() noexcept {
			return m_obj;
		}

		const std::vector<objective_encoding>& objective()const noexcept {
			return m_obj;
		}

		objective_encoding& objective(size_t idx) noexcept {
			return m_obj[idx];
		}

		const objective_encoding& objective(size_t idx)const noexcept{
			return m_obj[idx];
		}

		violation_type check_boundary_violation() {
			return global::ms_global->m_problem->check_boundary_violation(*this);
		}

		violation_type check_constraint_violation() {
			return global::ms_global->m_problem->check_constraint_violation(*this);
		}

		std::vector<real> & constraint_value() noexcept {
			return m_constraint_value;
		}

		const std::vector<real> & constraint_value()const noexcept {
			return m_constraint_value;
		}
		size_t number_violation() {
			size_t count = 0;
			for (auto &i : m_constraint_value)
				if (i > 0) ++count;
			return count;
		}

		void set_objective_infinite() {
	
			for (int i=0;i<m_obj.size();++i)
				m_obj[i] = (global::ms_global->m_problem->opt_mode(i) == optimization_mode::Minimization) ? std::numeric_limits<objective_encoding>::max() : std::numeric_limits<objective_encoding>::min();
			
		}
		const solution& solut() const noexcept{
			return *this;
		}
		solution& solut() noexcept {
			return *this;
		}
	protected:	
		variable_encoding m_var;
		std::vector<objective_encoding> m_obj;
		std::vector<real> m_constraint_value;
	};

	

}

#endif // !OFEC_SOLUTION_H
