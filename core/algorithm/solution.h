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
		typename ObjetiveType = real
	>
	class solution:public base{
	public:
		using objective_type = ObjetiveType;
		using variable_encoding = VariableEncoding;

		template<typename ... Args>
		solution(size_t no, Args&& ... args ):m_var(std::forward<Args>(args)...),m_obj(no){ }		
		solution(){}
		solution(const solution& rhs) :base(rhs), m_var(rhs.m_var), 
			m_obj(rhs.m_obj),m_constraint_value(rhs.m_constraint_value){}
		solution(solution&& rhs) :base(rhs),  m_var(std::move(rhs.m_var)), 
			m_obj(std::move(rhs.m_obj)), m_constraint_value(std::move(rhs.m_constraint_value)) {}
		solution(const variable_encoding& var, const objective<objective_type> &obj) :m_var(var), m_obj(obj) {}

		solution& operator =(const solution& rhs) {
			if (this == &rhs) return *this;
			m_var=rhs.m_var;
			m_obj=rhs.m_obj;
			m_constraint_value = rhs.m_constraint_value;
			return *this;
		}

		solution& operator =(solution&& rhs) {
			m_var = std::move(rhs.m_var);
			m_obj = std::move(rhs.m_obj);
			m_constraint_value = std::move(rhs.m_constraint_value);
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

		bool dominate( const objective<objective_type>& o) const{ //this soluton donimates objective o
			return  dominationship::Dominating == objective_compare(m_obj.vect(), o.vect(),  global::ms_global->m_problem->opt_mode());
		}

		bool dominate(const solution& s) const {//this solution donimates solution s
			return dominationship::Dominating == objective_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool dominate(const std::vector<objective_type>& o)const {//this solution donimates solution s
			return dominationship::Dominating == objective_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		bool dominate_equal(const objective<objective_type>& o)const { //this soluton weakly donimates objective o
			 dominationship r = objective_compare(m_obj.vect(),o.vect(), global::ms_global->m_problem->opt_mode());
			 return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool dominate_equal(const solution& s)const {//this solution weakly donimates solution s
			dominationship r = objective_compare(m_obj.vect(),s.m_obj.vect(), global::ms_global->m_problem->opt_mode() );
			return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool dominate_equal(const std::vector<objective_type>& o)const { //this soluton weakly donimates objective o
			dominationship r = objective_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		bool nondominate(const objective<objective_type>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const solution& s)const {//two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const std::vector<objective_type>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}
	 
		bool equal(const solution& rhs) const {
			return dominationship::Equal == objective_compare(m_obj.vect(), rhs.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool dominate(const objective<objective_type>& o, Compare comp) const { //this soluton donimates objective o
			return  dominationship::Dominating == comp(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool dominate(const solution& s, Compare comp) const {//this solution donimates solution s
			return dominationship::Dominating == comp(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool dominate(const std::vector<objective_type>& o, Compare comp)const {//this solution donimates solution s
			return dominationship::Dominating == comp(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool dominate_equal(const objective<objective_type>& o, Compare comp)const { //this soluton weakly donimates objective o
			dominationship r = comp(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		template<typename Compare>
		bool dominate_equal(const solution& s, Compare comp)const {//this solution weakly donimates solution s
			dominationship r = objective_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		template<typename Compare>
		bool dominate_equal(const std::vector<objective_type>& o, Compare comp)const { //this soluton weakly donimates objective o
			dominationship r = objective_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}

		template<typename Compare>
		bool nondominate(const objective<objective_type>& o, Compare comp)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool nondominate(const solution& s, Compare comp)const {//two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool nondominate(const std::vector<objective_type>& o, Compare comp)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == objective_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		template<typename Compare>
		bool equal(const solution& rhs, Compare comp) const {
			return dominationship::Equal == objective_compare(m_obj.vect(), rhs.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}

		bool same(const solution& x)const {//two solutions non-donimate with each other
			return global::ms_global->m_problem->same(*this, x);
		}

		void allocate_memory(size_t no, size_t nv) {
			m_var.resize(nv);
			m_obj.resize(no);
		}
		evaluation_tag evaluate(bool effective_eval=true) {
				
			return  global::ms_global->m_problem->evaluate(*this, caller::Algorithm, effective_eval);
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

		std::vector<objective_type>& get_objective() {
			return m_obj.vect();
		}

		const std::vector<objective_type>& get_objective()const {
			return m_obj.vect();
		}

		violation_type check_boundary_violation() {
			return global::ms_global->m_problem->check_boundary_violation(*this);
		}

		violation_type check_constraint_violation() {
			return global::ms_global->m_problem->check_constraint_violation(*this);
		}

		std::pair<double, std::vector<double>> & constraint_value() {
			return m_constraint_value;
		}

		const std::pair<double, std::vector<double>> & constraint_value()const {
			return m_constraint_value;
		}
		size_t number_violation() {
			size_t count = 0;
			for (auto &i : m_constraint_value.second)
				if (i > 0) ++count;
			return count;
		}

		void set_objective_infinite() {
	
			for (int i=0;i<m_obj.size();++i)
				m_obj[i] = (global::ms_global->m_problem->opt_mode(i) == optimization_mode::Minimization) ? std::numeric_limits<objective_type>::max() : std::numeric_limits<objective_type>::min();
			
		}

	protected:	
		variable_encoding m_var;
		objective<objective_type> m_obj;
		std::pair<double, std::vector<double>> m_constraint_value;
	};

	
}

#endif // !OFEC_SOLUTION_H
