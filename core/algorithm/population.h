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
#include "../measure/measure.h"
#include "../../utility/nondominated_sorting/filter_sort/filter_sort.h"
#include "../../utility/functional.h"
namespace OFEC {
	template<typename>  class multi_population;

	template<typename Individual>
	class population : public algorithm {
	public:
		template<typename> friend class multi_population;
		using individual_type = Individual;
		using iterator_type = typename std::vector<std::shared_ptr< Individual>>::iterator;
		//element access
		size_t size()const noexcept{
			return m_pop.size();
		}
		const Individual& operator[](size_t i) const {
			return *m_pop[i];
		}
		Individual& operator[](size_t i) {
			return *m_pop[i];
		}
		const std::vector<std::unique_ptr<Individual>>& best();
		const std::vector<std::unique_ptr<Individual>>& worst();
		const std::vector<int> order(int idx); // get individual(s) with order idx
		int iteration() { return m_iter; }

		//update	
		void update_best();
		void update_worst();
		
		void handle_evaluation_tag(evaluation_tag tag) {}

		//constructors and members
		population() = default;
		template<typename ... Args>
		population(size_t n, Args&& ...args);
		population(const population &rhs);
		population& operator=(const population &rhs);
		population(population&&rhs);
		population& operator=(population &&rhs);

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

		evaluation_tag initialize_pop(bool effective_eval = true); //a uniformly distributed initialization by default
		template<typename Fun, typename Problem, typename... Args>
		evaluation_tag initialize_pop(Fun fun, const Problem *pro, Args && ... args);

		std::map<double, size_t> nearest_neighbour(size_t idx, int k = 1);

		evaluation_tag initialize();
		void record();	 
		void reset_improved_flag();
		double rank(const typename Individual::solution_type &x);
		int number_PF() { return m_order.rbegin()->first; }
		int id() { return m_id; }
		void resize(size_t size);
	protected:
		virtual evaluation_tag evolve_() { return evaluation_tag::Normal; }
		void update_best(const Individual &);
		void update_worst(const Individual &);
	protected:
		int m_iter = 0;			// the current number of iterations
		int m_id;
		std::vector<std::unique_ptr<Individual>> m_pop;
		std::vector<std::unique_ptr<Individual>> m_best, m_worst;
		std::multimap<int, int> m_order;
		bool m_ordered = false, m_best_updated = false, m_worst_updated = false;
	private:
		int m_max_id;
	};

	template<typename Individual>
	evaluation_tag population<Individual>::evolve() {
		return evolve_();
		m_ordered = false, m_best_updated = false, m_worst_updated = false;
	}
	template<typename Individual>
	const std::vector<std::unique_ptr<Individual>>& population<Individual>::best() {
		if (!m_best_updated) {
			update_best();
		}
		return m_best;
	}
	template<typename Individual>
	const std::vector<std::unique_ptr<Individual>>& population<Individual>::worst() {
		if (!m_worst_updated) {
			update_worst();
		}
		return m_worst;
	}

	template<typename Individual>
	const std::vector<int> population<Individual>::order(int idx) {
		if (!m_ordered)
			sort(NS::filter_sort);
		auto range_ = m_order.equal_range(idx);
		std::vector<int> inds;
		for (auto i = range_.first; i != range_.second; ++i)
			inds.push_back(i->second);
		return inds;
	}

	template<typename Individual>
	void population<Individual>::update_best() {
		if (m_best_updated)
			return;
		m_best.clear();
		std::vector<bool> flag(m_pop.size(), true);

		for (size_t j = 0; j<m_pop.size(); j++) {
			for (size_t i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (m_pop[j]->dominate(*m_pop[i])) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i < m_pop.size(); i++) {
			if (flag[i])
				m_best.push_back(std::unique_ptr<Individual>(new Individual(*m_pop[i])));
		}
		m_best_updated = true;
	}

	template<typename Individual>
	void population<Individual>::update_worst() {
		if (m_worst_updated)
			return;
		m_worst.clear();
		std::vector<bool> flag(m_pop.size(), true);

		for (size_t j = 0; j<m_pop.size(); j++) {
			for (size_t i = 0; i<m_pop.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (m_pop[i]->dominate(m_pop[j])) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i<m_pop.size(); i++) { 
			if (flag[i])
				m_worst.push_back(std::unique_ptr<Individual>(new Individual(*m_pop[i])));
		}
		m_worst_updated = true;
	}

	template<typename Individual>
	void population<Individual>::update_best(const Individual &x) {
		if (m_best.empty()) {
			m_best.push_back(std::unique_ptr<Individual>(new Individual(x)));
			return;
		}
		bool first = true;
		// check dominated case
		for (auto i = m_best.begin(); i != m_best.end(); i++) {
			if (first && x.dominate(**i)) {//**i<x
				**i = x;
				first = false;
			}
			else if (!first && x.dominate(**i)) {//**i<x
				i = m_best.erase(i);
				i--;
			}
		}
		if (!first) return;
		//check equal case	
		for (auto i = m_best.begin(); i != m_best.end(); i++) {
			if (x.equal(**i) && !((*i)->same(x))) { //**i == x 
				m_best.push_back(std::unique_ptr<Individual>(new Individual(x)));
				return;
			}
		}
		//check non-dominated case	
		for (auto i = m_best.begin(); i != m_best.end(); i++) {
			if (!((*i)->nondominate(x))) return;
		}
		m_best.push_back(std::unique_ptr<Individual>(new Individual(x)));

		m_best_updated = true;
		return;
	}

	template<typename Individual>
	void population<Individual>::update_worst(const Individual &x) {
		if (m_worst.empty()) {
			m_worst.push_back(std::unique_ptr<Individual>(new Individual(x)));
			return;
		}

		bool first = true;
		// check dominated case
		for (auto i = m_worst.begin(); i != m_worst.end(); i++) {
			if (first && (*i)->dominate(x)) {//**i<x
				**i = x;
				first = false;
			}
			else if (!first && (*i)->dominate(x)) {//**i<x
				i = m_worst.erase(i);
				i--;
			}
		}
		if (!first) return;
		//check equal case	
		for (auto i = m_worst.begin(); i != m_worst.end(); i++) {
			if (x.equal(**i) && !((*i)->same(x))) { //**i == x 
				m_worst.push_back(std::unique_ptr<Individual>(new Individual(x)));
				return;
			}
		}
		//check non-dominated case	
		for (auto i = m_worst.begin(); i != m_worst.end(); i++) {
			if (!((*i)->nondominate(x))) return;
		}
		m_worst.push_back(std::unique_ptr<Individual>(new Individual(x)));

		m_worst_updated = true;
		return;
	}

	template<typename Individual>
	template<typename ... Args>
	population<Individual>::population(size_t n, Args&& ... args) :algorithm(std::string()), m_pop(n) {
		size_t size_obj = global::ms_global->m_problem->objective_size();
		for (auto& i : m_pop)
			i = std::move(std::unique_ptr<Individual>(new Individual(size_obj, std::forward<Args>(args)...)));
	}

	template<typename Individual>
	population<Individual>::population(const population &rhs) :algorithm(std::string()), m_iter(rhs.m_iter), m_id(rhs.m_id),\
	m_best(rhs.m_best), m_worst(rhs.m_worst), m_order(rhs.m_order), m_ordered(rhs.m_ordered), m_best_updated(rhs.m_best_updated),\
	m_worst_updated(rhs.m_worst_updated), m_max_id(rhs.m_max_id) {
		for (size_t i = 0; i < rhs.size(); i++)	{
			m_pop.push_back(std::unique_ptr<Individual>(new Individual(rhs[i])));
		}
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(const population &rhs) {
		
		m_iter = rhs.m_iter;
		m_id = rhs.m_id;
		for (size_t i = 0; i < rhs.size(); i++) {
			m_pop.push_back(std::unique_ptr<Individual>(new Individual(rhs[i])));
		}
		update_best();
		update_worst();

		m_order = rhs.m_order;
		m_ordered = rhs.m_ordered;
		m_best_updated = rhs.m_best_updated;
		m_worst_updated = rhs.m_worst_updated;
		m_max_id = rhs.m_max_id;
		return *this;
	}

	template<typename Individual>
	population<Individual>::population(population&&rhs) :algorithm(std::string()), m_iter(rhs.m_iter), m_id(rhs.m_id), m_pop(std::move(rhs.m_pop)), \
		m_best(std::move(rhs.m_best)), m_worst(std::move(rhs.m_worst)), m_order(std::move(rhs.m_order)), m_ordered(rhs.m_ordered),\
		m_best_updated(rhs.m_best_updated),	m_worst_updated(rhs.m_worst_updated), m_max_id(rhs.m_max_id) {
		
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(population &&rhs) {
		m_iter = rhs.m_iter;
		m_id = rhs.m_id;
		m_pop = std::move(rhs.m_pop);
		m_best = std::move(rhs.m_best);
		m_worst = std::move(rhs.m_worst);
		m_order = std::move(rhs.m_order);
		m_ordered = rhs.m_ordered;
		m_best_updated = rhs.m_best_updated;
		m_worst_updated = rhs.m_worst_updated;
		m_max_id = rhs.m_max_id;
		return *this;
	}

	template<typename Individual>
	void population<Individual>::reset() {

		m_iter = 0;
		m_best.clear();
		m_worst.clear();
		m_pop.clear();
		m_order.clear();
		
	}

	template<typename Individual>
	evaluation_tag population<Individual>::initialize_pop(bool effective_eval) {
		evaluation_tag tag;
		for (int i = 0; i < m_pop.size(); ++i) {
			m_pop[i]->initialize(i);
			tag = m_pop[i]->evaluate(effective_eval);
			update_best(*m_pop[i]);
			update_worst(*m_pop[i]);
			if (tag != evaluation_tag::Normal) break;
		}
		m_ordered = false;
		m_best_updated = m_worst_updated = true;
		return tag;
	}
	template<typename Individual>
	template<typename Fun, typename Problem, typename... Args>
	evaluation_tag  population<Individual>::initialize_pop(Fun fun, const Problem* pro, Args&& ... args) {

		return fun(m_pop, pro, std::forward<Args>(args)...);

	}

	template<typename Individual>
	template<typename Fun, typename ... Args>
	void population<Individual>::sort(Fun fun, Args&& ... args) {
		const size_t N = m_pop.size();
		std::vector<std::vector<double>> data(N);
		for (size_t i = 0; i < N; ++i)
			data[i] = m_pop[i]->objective();
		std::vector<int> rank(N);
		std::pair<int, int> measures(0, 0);
		fun(data, rank, measures, std::forward<Args>(args)...);
		m_order.clear();
		for (size_t i = 0; i < N; i++) {
			m_order.insert(std::pair<int,int>(rank[i], m_pop[i]->id()));
			m_pop[i]->set_rank(rank[i]);
		}
		m_ordered = true;
	}

	template<typename Individual>
	std::map<double, size_t> population<Individual>::nearest_neighbour(size_t idx, int k) {
		//TODO return k nearest neighbour of individual idx together with the distance
		double Min_dis;
		size_t index;
		size_t count = 0;
		std::vector<double> dis(size());
		for (size_t i = 0; i < size(); ++i) {
			if (idx == i) {
				dis[i] = 0;
				continue;
			}
			dis[i] = m_pop[idx]->variable_distance(*m_pop[i]);
		}

		for (size_t i = 0; i < size(); ++i) {
			if (i == idx) continue;
			++count;
			if (count == 1) {
				Min_dis = dis[i];
				index = i;
			}
			else if (dis[i]<Min_dis) {
				Min_dis = dis[i];
				index = i;
			}
		}
		
		std::map<double, size_t> result;
		result[Min_dis] = index;

		return result;
	}

	template<typename Individual>
	evaluation_tag population<Individual>::evaluate() {
		evaluation_tag tag = evaluation_tag::Normal;
		for (auto &i : m_pop) {
			tag = i->evaluate();
			if (tag != evaluation_tag::Normal) break;
		}
		return tag;
	}

	template<typename Individual>
	void population<Individual>::resize_objective(int n) {
		for (auto &i : m_pop)
			i.resize_objective(n);
	}
	template<typename Individual>
	void population<Individual>::resize_variable(int n) {
		for (auto &i : m_pop)
			i.resize_variable(n);
	}

	template<typename Individual>
	evaluation_tag population<Individual>::initialize() {
		return initialize_pop();
	}

	template<typename Individual>
	void population<Individual>::record() {
		if (!m_best_updated) 
			update_best();
		
	}

	template<typename Individual>
	void population<Individual>::reset_improved_flag() {
		for (auto &i : m_pop) {
			i->set_improved_flag(false);
		}
	}

	template<typename Individual>
	double population<Individual>::rank(const typename Individual::solution_type &x) {
		int i = 0;
		while (1) {
			auto l = m_order.lower_bound(i), u = m_order.upper_bound(i);
			while (l != u) {
				dominationship r = objective_compare(x.objective(), m_pop[l->second]->objective(), global::ms_global->m_problem->opt_mode()).first;
				if(r == dominationship::Dominating) return i - 0.5;
				if (r == dominationship::Dominated) break;
				++l;
			}
			if (l == u) return i;			
			if (u == m_order.end()) return i + 0.5;
			++i;
		}		 
	}

	template<typename Individual>
	void population<Individual>::resize(size_t size) {
		if (size > m_pop.size()) {
			for (auto i = m_pop.size(); i < size; i++) {
				m_pop.push_back(unique_ptr<Individual>(new Individual()));
			}
		}
		else if (size < m_pop.size()) {
			m_pop.resize(size);
		}
		
		m_ordered = m_best_updated = m_worst_updated = false;
	}

}

#endif // !OFEC_POPULATION_H
