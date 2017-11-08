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
* class population manages a set of individuals.
*
*********************************************************************************/

#ifndef OFEC_POPULATION_H
#define OFEC_POPULATION_H

#include "../global.h"

namespace OFEC {
	template<typename>  class multi_population;

	template<typename Individual>
	class population: public algorithm{
		template<typename> friend class multi_population;
		using individual_type = Individual;
	public:
		using iterator_type = typename std::vector<std::shared_ptr< Individual>>::iterator;
		//element access
		int size()const {
			return m_pop.size();
		}
		const std::vector<int> order(int idx) const { // get individual(s) with order idx
			auto range_ = m_order.equal_range(idx);
			std::vector<int> inds;
			for (auto i = range_.first; i != range_.second; ++i)
				inds.push_back(i->second);
			return inds;
		}
		const Individual& operator[](int i) const {
			return *m_pop[i];
		}
		Individual& operator[](int i)  {
			return *m_pop[i];
		}
		std::vector<std::unique_ptr<typename Individual::solution_type>>& archive() {
			return m_arc;
		}
		const std::vector<std::shared_ptr<Individual>>& best();
		const std::vector<std::shared_ptr<Individual>>& worst();
		int iteration() { return m_iter; }

		//update	
		void update_best();
		void update_worst();
		virtual bool update_archive(const typename Individual::solution_type &x);
		void handle_evaluation_tag(evaluation_tag tag);

		//constructors and members
		population()=default;
		template<typename ... Args>
		population(int n, Args&& ...args);
		population(const population &rhs);
		population& operator=(const population &rhs);
		population(population&&rhs);
		population& operator=(population&&rhs);
		
		//operations
		iterator_type operator +(const population &p);
		iterator_type operator +(population &&p);
		iterator_type operator +(const Individual &p);
		iterator_type operator +(Individual &&p);
		iterator_type operator +(std::vector<Individual*> &&p);
		iterator_type operator +(const std::vector<Individual*> &p);
		iterator_type operator +(std::vector<Individual> &&p);
		iterator_type operator +(const std::vector<Individual> &p);
		iterator_type operator +(int n);// add n individuals by default constructor 

		iterator_type operator-(std::vector<int> &id); //remove individuals by id
		iterator_type operator-(int n); // remove n worst individuals by default

		template<typename Fun, typename ... Args>
		void sort(Fun fun, Args&& ... args);		

		void resize_objective(int n);
		void resize_variable(int n);

		evaluation_tag evaluate(); // evaluate each individual 
		evaluation_tag evolve();
				
		void reset(); // delete all individuals
		
		double mean(int oidx);	//mean value of the oidx-th objective
		double variance(int oidx, double mean); //variance of the oidx-th objective

		virtual void initialize(); //a uniformly distributed initialization by default
		template<typename Fun, typename Problem, typename... Args>
		void initialize(Fun fun, const Problem* pro, Args&& ... args);
	protected:
		virtual evaluation_tag evolve_() { return evaluation_tag::Normal; }
	protected:
		int m_iter = 0;			// the current number of iterations
		int m_id;				
		std::vector<std::shared_ptr<Individual>> m_pop; 
		std::vector<std::shared_ptr<Individual>> m_best,m_worst;
		std::vector<std::unique_ptr<typename Individual::solution_type>> m_arc;// external archive for solutions
		std::multimap<int,int> m_order; 

		bool m_ordered = false, m_best_updated = false, m_worst_updated = false;
	private:
		int m_max_id;
	};

	template<typename Individual>
	evaluation_tag population<Individual>::evolve() {
		m_ordered = false;
		m_best_updated = false;
		m_worst_updated = false;
		return evolve_();
	}
	template<typename Individual>
	const std::vector<std::shared_ptr<Individual>>& population<Individual>::best() {
		if (!m_best_updated) {
			update_best();
		}
		return m_best;
	}
	template<typename Individual>
	const std::vector<std::shared_ptr<Individual>>& population<Individual>::worst() {
		if (!m_worst_updated) {
			update_worst();
		}
		return m_worst;
	}

	template<typename Individual>
	void population<Individual>::update_best() {
		m_best.clear();
	 std::vector<bool> flag(m_pop.size(), true);
	
		for (size_t j = 0; j<m_pop.size(); j++) {
			for (size_t i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (*m_pop[j]>*m_pop[i]) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i<m_pop.size(); i++) { if (flag[i]) m_best.push_back(m_pop[i]); }
		m_best_updated = true;
	}

	template<typename Individual>
	void population<Individual>::update_worst() {
		m_worst.clear();
	 std::vector<bool> flag(m_pop.size(), true);

		for (size_t j = 0; j<m_pop.size(); j++) {
			for (size_t i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (*m_pop[j]<*m_pop[i]) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i<m_pop.size(); i++) { if (flag[i]) m_worst.push_back(m_pop[i]); }
		m_worst_updated = true;
	}

	template<typename Individual>
	bool population<Individual>::update_archive(const typename Individual::solution_type &x) {
		bool first = true;
		// check dominated case
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (first && x.dominate(**i)) {//**i<x
				**i = x;
				first = false;
			}
			else if (!first && x.dominate(**i)) {//**i<x
				i = m_arc.erase(i);
				i--;
			}
		}
		if (!first) return true;
		//check equal case	
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (x.equal(**i) && !((*i)->same(x))) { //**i == x 
				m_arc.push_back(std::unique_ptr<typename Individual::solution_type>(new typename Individual::solution_type(x)));
				return true;
			}
		}
		//check non-dominated case	
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (!(*i)->nondominate(x)) return false;
		}
		m_arc.push_back(std::move(std::unique_ptr<typename Individual::solution_type>(new typename Individual::solution_type(x))));

		return true;
	}

	template<typename Individual>
	template<typename ... Args>
	population<Individual>::population(int n, Args&& ... args) : m_pop(n) {
		size_t size_obj = global::ms_global->m_problem->objective_size();
		for (auto& i : m_pop)
			i = std::move(std::unique_ptr<Individual>(new Individual(size_obj, std::forward<Args>(args)...)));
		initialize();
	}

	template<typename Individual>
	population<Individual>::population(const population &rhs) {
	
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(const population &rhs) {}

	template<typename Individual>
	population<Individual>::population(population&&rhs) {
	
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(population&&rhs) {

	}

	template<typename Individual>
	void population<Individual>::initialize() {
		for (size_t i = 0; i < m_pop.size(); ++i) {
			m_pop[i]->initialize(i);
		}
			
	}
	template<typename Individual>
	template<typename Fun, typename Problem, typename... Args>
	void  population<Individual>::initialize(Fun fun, const Problem* pro, Args&& ... args) {

		fun(m_pop, pro, std::forward<Args>(args)...);

	}

	template<typename Individual>
	template<typename Fun, typename ... Args>
	void population<Individual>::sort(Fun fun, Args&& ... args) {
		const size_t N = m_pop.size();
		std::vector<std::vector<typename Individual::solution_type::objective_type> * > obj(N);
		for (size_t i = 0; i < N; ++i)
			obj[i] = &(m_pop[i]->get_objective());
		std::vector<int> rank(N);
		fun(obj, rank, std::forward<Args>(args)...);
	}


}

#endif // !OFEC_POPULATION_H
