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
* It takes a variable encoding, an objective value type, an objective compaison 
* method, and an objecitve distance method as parameters to generate a solution.
*
*********************************************************************************/

#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H
#include <utility>

#include "encoding.h"
#include "../definition.h"
#include "../global.h"

namespace  OFEC {

	template<typename VariableEncoding = variable<real>, 
		typename ObjetiveType = real, 
		typename ObjetiveCompare = objective_compare<ObjetiveType>
	>
	class solution:public base{
	public:
		using objective_type = ObjetiveType;
		using variable_encoding = VariableEncoding;

		template<typename ... Args>
		solution(size_t no, Args&& ... args ):m_var(std::forward<Args>(args)...),m_obj(no){ }		
		solution(){}
		solution(const solution& rhs) :base(rhs),m_compare(rhs.m_compare), m_var(rhs.m_var), 
			m_obj(rhs.m_obj),m_constraint_value(rhs.m_constraint_value),m_violation(rhs.m_violation){}
		solution(solution&& rhs) :base(rhs), m_compare(std::move(rhs.m_compare)), m_var(std::move(rhs.m_var)), 
			m_obj(std::move(rhs.m_obj)), m_constraint_value(std::move(rhs.m_constraint_value)),	m_violation(std::move(rhs.m_violation)) {}
		solution(const variable_encoding& var, const objective<objective_type> &obj) :m_var(var), m_obj(obj) {}

		solution& operator =(const solution& rhs) {
			if (this == &rhs) return *this;
			m_var=rhs.m_var;
			m_obj=rhs.m_obj;
			m_compare = rhs.m_compare;
			m_constraint_value = rhs.m_constraint_value;
			m_violation = rhs.m_violation;
			return *this;
		}

		solution& operator =(solution&& rhs) {
			m_var = std::move(rhs.m_var);
			m_obj = std::move(rhs.m_obj);
			m_compare = std::move(rhs.m_compare);
			m_constraint_value = std::move(rhs.m_constraint_value);
			m_violation = std::move(rhs.m_violation);
			return *this;
		}

		virtual void initialize() {
			global::ms_global->m_problem->initialize_solution(*this);
		}

		template<typename Initializer, typename... Args>
		void initialize(Initializer f, Args&&... args) {
			f(std::forward<Args>(args)...);
		}
		size_t variable_size() const {		return m_var.size();	}
		size_t objective_size() const { return m_obj.size(); }

		void resize_objective(int n) { m_obj.resize(n); }
		void resize_variable(int n) { m_var.resize(n); }

		bool operator>( const objective<objective_type>& o) const{ //this soluton donimates objective o
			return  dominationship::Dominating == m_compare(m_obj.vect(), o.vect(),  global::ms_global->m_problem->opt_mode());
		}
		bool operator>(const solution& s) const {//this solution donimates solution s
			return dominationship::Dominating == m_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator>(const std::vector<objective_type>& o)const {//this solution donimates solution s
			return dominationship::Dominating == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		bool operator>=(const objective<objective_type>& o)const { //this soluton weakly donimates objective o
			 dominationship r = m_compare(m_obj.vect(),o.vect(), global::ms_global->m_problem->opt_mode());
			 return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool operator>=(const solution& s)const {//this solution weakly donimates solution s
			dominationship r = m_compare(m_obj.vect(),s.m_obj.vect(), global::ms_global->m_problem->opt_mode() );
			return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool operator>=(const std::vector<objective_type>& o)const { //this soluton weakly donimates objective o
			dominationship r = m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		bool operator<(const objective<objective_type>& o)const { //this solution is donimated by o
			return  dominationship::Dominated == m_compare(m_obj.vect(),o.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator<(const solution& s)const {//this solution is donimated s
			return  dominationship::Dominated == m_compare(m_obj.vect(),s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator<(const std::vector<objective_type>& o)const { //this solution is donimated by o
			return  dominationship::Dominated == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		bool nondominate(const objective<objective_type>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const solution& s)const {//two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const std::vector<objective_type>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}
	 
		bool operator ==(const solution& rhs) {
			return dominationship::Equal == m_compare(m_obj.vect(), rhs.m_obj.vect(), global::ms_global->m_problem->opt_mode());
			//if (dominationship::Equal != m_compare(m_obj.vect(), rhs.vect(), global::ms_global->m_problem->opt_mode()))
			//	return false;
			//return global::ms_global->m_problem->same(*this, &rhs);
		}

		bool same(const solution& x)const {//two solutions non-donimate with each other
			return global::ms_global->m_problem->same(*this, x);
		}

		void allocate_memory(size_t no, size_t nv) {
			m_var.resize(nv);
			m_obj.resize(no);
		}
		evaluation_tag evaluate(bool effective_eval=true) {
			evaluation_tag tag = evaluation_tag::Normal;

			if(m_violation!=violation_type::Boundary)
			tag = global::ms_global->m_problem->evaluate(*this, caller::Algorithm, effective_eval);
			else tag = evaluation_tag::Infeasible;

			return tag;
		}

		double objective_distance(const solution& rhs) const {			
			if (objective_size() == 1) return fabs(m_obj[0] - rhs.m_obj[0]);

			return euclidean_distance(m_obj.begin(), m_obj.end(), rhs.m_obj.begin());
		}

		double variable_distance(const solution& rhs) const {			
			return global::ms_global->m_problem->variable_distance(*this, &rhs);
		}

		double variable_distance(const variable_encoding& x) const {
			return global::ms_global->m_problem->variable_distance(m_var, x);
		}

		const variable_encoding& get_variable()const {
			return m_var;
		}

		variable_encoding& get_variable(){
			return m_var;
		}

		vector<objective_type>& get_objective() {
			return m_obj.vect();
		}

		const vector<objective_type>& get_objective()const {
			return m_obj.vect();
		}

		violation_type check_violation() {
			return m_violation=global::ms_global->m_problem->check_violation(*this);
		}

		void constraint_value() {
			global::ms_global->m_problem->constraint_value(m_constraint_value);
		}

		void set_objective_infinite() {
	
			for (int i=0;i<m_obj.size();++i)
				m_obj[i] = (global::ms_global->m_problem->opt_mode(i) == optimization_mode::Minimization) ? std::numeric_limits<objective_type>::max() : std::numeric_limits<objective_type>::min();
			
		}

	protected:	
		ObjetiveCompare m_compare;
		variable_encoding m_var;
		objective<objective_type> m_obj;
		std::pair<double, std::vector<double>> m_constraint_value;
		violation_type m_violation = violation_type::None;
	};

	
}

#endif // !OFEC_SOLUTION_H
