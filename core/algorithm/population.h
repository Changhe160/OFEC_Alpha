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

#include "algorithm.h"

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
		const std::vector<std::shared_ptr<Individual>>& order(int idx) const { // get the order of the idx-th objecitve
			return m_order[idx];
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
		population(int n);
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


		void sort();		
		void rank(); //TODO: compare the ranking method in NSGAII with our own method 
		double rank(const typename Individual::solution_type &s); // get rank of s in terms of a ranked population

		void resize_objective(int n);
		void resize_variable(int n);

		evaluation_tag evaluate(); // evaluate each individual 
		evaluation_tag evolve();
		
		
		void reset(); // delete all individuals
		
		double mean(int oidx);	//mean value of the oidx-th objective
		double variance(int oidx, double mean); //variance of the oidx-th objective

		virtual void initialize(); //a uniformly distributed initialization by default
	protected:
		virtual evaluation_tag evolve_() { return evaluation_tag::Normal; }
	protected:
		int m_iter = 0;			// the current number of iterations
		int m_id;				
		std::vector<std::shared_ptr<Individual>> m_pop; 
		std::vector<std::shared_ptr<Individual>> m_best,m_worst;
		std::vector<std::unique_ptr<typename Individual::solution_type>> m_arc;// external archive for solutions
		std::vector<std::vector<std::shared_ptr<Individual>>> m_order; //sorted order according to each objective

		bool m_ordered = false, m_best_updated = false, m_worst_updated = false, m_ranked = false;
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
		vector<bool> flag(m_pop.size(), true);
	
		for (int j = 0; j<m_pop.size(); j++) {
			for (int i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (*m_pop[j]>*m_pop[i]) {
					flag[i] = false;
				}
			}
		}
		for (int i = 0; i<m_pop.size(); i++) { if (flag[i]) m_best.push_back(m_pop[i]); }
		m_best_updated = true;
	}

	template<typename Individual>
	void population<Individual>::update_worst() {
		m_worst.clear();
		vector<bool> flag(m_pop.size(), true);

		for (int j = 0; j<m_pop.size(); j++) {
			for (int i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (*m_pop[j]<*m_pop[i]) {
					flag[i] = false;
				}
			}
		}
		for (int i = 0; i<m_pop.size(); i++) { if (flag[i]) m_worst.push_back(m_pop[i]); }
		m_worst_updated = true;
	}

	template<typename Individual>
	bool population<Individual>::update_archive(const typename Individual::solution_type &x) {
		bool first = true;
		// check dominated case
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (first && **i<x) {
				**i = x;
				first = false;
			}
			else if (!first && **i<x) {
				i = m_arc.erase(i);
				i--;
			}
		}
		if (!first) return true;
		//check equal case	
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (**i == x && !((*i)->same(x))) {
				m_arc.push_back(unique_ptr<Individual::solution_type>(new Individual::solution_type(x)));
				return true;
			}
		}
		//check non-dominated case	
		for (auto i = m_arc.begin(); i != m_arc.end(); i++) {
			if (!(*i)->nondominate(x)) return false;
		}
		m_arc.push_back(move(unique_ptr<Individual::solution_type>(new Individual::solution_type(x))));

		return true;
	}

	template<typename Individual>
	population<Individual>::population(int n) {
		
	}

	template<typename Individual>
	population<Individual>::population(const population &rhs) {}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(const population &rhs) {}

	template<typename Individual>
	population<Individual>::population(population&&rhs) {}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(population&&rhs) {

	}

	template<typename Individual>
	void population<Individual>::initialize() {
		for (size_t i = 0; i < m_pop.size(); ++i) {
			m_pop[i]->initialize(i);
		}
			
	}
}

#endif // !OFEC_POPULATION_H
