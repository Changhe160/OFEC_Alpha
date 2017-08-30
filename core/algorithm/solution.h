#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H
#include <utility>

#include "encoding.h"
#include "../definition.h"
#include "../global.h"

/*
class solution takes a variable encoding, an objective encoding, and an objective compaison methord as parameters 
to generate a solution. 
*/
namespace  OFEC {

	template<typename VariableEncoding = variable<double>, 
		typename ObjetiveType = double, 
		typename ObjetiveCompare = objective_compare<ObjetiveType>,
		typename ObjetiveDistance = euclidean_distance<vector<ObjetiveType>::const_iterator, vector<ObjetiveType>::const_iterator>
	>
	class solution:public base{
	public:
		using objective_type = ObjetiveType;
		using variable_encoding = VariableEncoding;

		template<typename ... Args>
		solution(size_t no, Args&& ... args ):m_var(std::forward<Args>(args)...),m_obj(no){ }		
		solution(){}
		solution(const solution& rhs) :base(rhs),m_compare(rhs.m_compare), m_var(rhs.m_var), 
			m_obj(rhs.m_obj),m_objective_distance(rhs.m_objective_distance),m_constraint_value(rhs.m_constraint_value),
			m_violation(rhs.m_violation){}
		solution(solution&& rhs) :base(rhs), m_compare(std::move(rhs.m_compare)), m_var(std::move(rhs.m_var)), 
			m_obj(std::move(rhs.m_obj)), m_objective_distance(std::move(rhs.m_objective_distance)), m_constraint_value(std::move(rhs.m_constraint_value)),
			m_violation(std::move(rhs.m_violation)) {}
		solution(const VariableEncoding& var, const objective<ObjetiveType> &obj) :m_var(var), m_obj(obj) {}

		solution& operator =(const solution& rhs) {
			if (this == &rhs) return *this;
			m_var=rhs.m_var;
			m_obj=rhs.m_obj;
			m_compare = rhs.m_compare;
			m_constraint_value = rhs.m_constraint_value;
			m_objective_distance = rhs.m_objective_distance;
			m_violation = rhs.m_violation;
			return *this;
		}

		solution& operator =(solution&& rhs) {
			m_var = std::move(rhs.m_var);
			m_obj = std::move(rhs.m_obj);
			m_compare = std::move(rhs.m_compare);
			m_constraint_value = std::move(rhs.m_constraint_value);
			m_objective_distance = std::move(rhs.m_objective_distance);
			m_violation = std::move(rhs.m_violation);
			return *this;
		}

		void initialize(int idx = 0, int max_idx = 0) {
			global::ms_global->m_problem->initialize(*this, idx, max_idx);
		}

		int variable_size() {		return m_var.size();	}
		int objective_size() { return m_obj.size(); }

		bool operator>( const objective<ObjetiveType>& o) const{ //this soluton donimates objective o
			return  dominationship::Dominating == m_compare(m_obj.vect(), o.vect(),  global::ms_global->m_problem->opt_mode());
		}
		bool operator>(const solution& s) const {//this solution donimates solution s
			return dominationship::Dominating == m_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator>(const std::vector<ObjetiveType>& o)const {//this solution donimates solution s
			return dominationship::Dominating == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		bool operator>=(const objective<ObjetiveType>& o)const { //this soluton weakly donimates objective o
			 dominationship r = m_compare(m_obj.vect(),o.vect(), global::ms_global->m_problem->opt_mode());
			 return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool operator>=(const solution& s)const {//this solution weakly donimates solution s
			dominationship r = m_compare(m_obj.vect(),s.m_obj.vect(), global::ms_global->m_problem->opt_mode() );
			return r == dominationship::Dominating || r == dominationship::Equal;
		}
		bool operator>=(const std::vector<ObjetiveType>& o)const { //this soluton weakly donimates objective o
			dominationship r = m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
			return r == dominationship::Dominating || r == dominationship::Equal;
		}


		bool operator<(const objective<ObjetiveType>& o)const { //this solution is donimated by o
			return  dominationship::Dominated == m_compare(m_obj.vect(),o.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator<(const solution& s)const {//this solution is donimated s
			return  dominationship::Dominated == m_compare(m_obj.vect(),s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool operator<(const std::vector<ObjetiveType>& o)const { //this solution is donimated by o
			return  dominationship::Dominated == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}

		bool nondominate(const objective<ObjetiveType>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), o.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const solution& s)const {//two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), s.m_obj.vect(), global::ms_global->m_problem->opt_mode());
		}
		bool nondominate(const std::vector<ObjetiveType>& o)const { //two solutions non-donimate with each other
			return  dominationship::Non_dominated == m_compare(m_obj.vect(), o, global::ms_global->m_problem->opt_mode());
		}
	 
		bool operator ==(const solution& rhs) {
			if (dominationship::Equal != m_compare(m_obj.vect(), rhs.vect(), global::ms_global->m_problem->opt_mode()))
				return false;
			return global::ms_global->m_problem->equal(*this, &rhs);
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
			return m_objective_distance(m_obj.begin(), m_obj.end(), rhs.m_obj.begin());
		}

		double variable_distance(const solution& rhs) const {			
			return global::ms_global->m_problem->variable_distance(*this, &rhs);
		}

		vector<ObjetiveType>& objective_vect() {
			return m_obj.vect();
		}

		const vector<ObjetiveType>& objective_vect()const {
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

		void repair() {
			global::ms_global->m_problem->repair(*this);
		}


	protected:	
		ObjetiveCompare m_compare;
		VariableEncoding m_var;	
		objective<ObjetiveType> m_obj;
		ObjetiveDistance m_objective_distance;
		std::pair<double, std::vector<double>> m_constraint_value;
		violation_type m_violation = violation_type::None;
	};

	
}

#endif // !OFEC_SOLUTION_H
