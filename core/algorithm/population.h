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
#include "../../utility/functional.h"
#include "../../utility/nondominated_sorting/fast_sort.h"
#include "../../utility/nondominated_sorting/filter_sort.h"

namespace OFEC {
	template<typename>  class multi_population;

	template<typename Individual>
	class population {
	public:
		virtual ~population() = default;
		template<typename> friend class multi_population;
		using individual_type = Individual;
		using solution_type = typename individual_type::solution_type;
		using objective_type = typename solution_type::objective_encoding;
		using iterator_type = typename std::vector<std::unique_ptr<Individual>>::iterator;
		//element access
		size_t size()const noexcept{
			return m_inds.size();
		}
		const Individual& operator[](size_t i) const {
			return *m_inds[i];
		}
		Individual& operator[](size_t i) {
			return *m_inds[i];
		}
		const std::vector<std::unique_ptr<solution_type>>& best() {
			m_best_monitor = true;
			if (!m_best_updated) {
				update_best();
			}
			return m_best;
		}
		const std::vector<std::unique_ptr<solution_type>>& best() const { return m_best; }
		const std::vector<std::unique_ptr<solution_type>>& worst() {
			m_worst_monitor = true;
			if (!m_worst_updated) {
				update_worst();
			}
			return m_worst;
		}
		const std::vector<std::unique_ptr<solution_type>>& worst() const { return m_worst; }
		const std::vector<int> order(int idx); // get individual(s) with order idx
		int iteration() const { return m_iter; }

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
		void add(Individual&& p);
		void remove(size_t idx);

		iterator_type operator-(std::vector<int> &id); //remove individuals by id
		iterator_type operator-(int n); // remove n worst individuals by default

		virtual void sort();

		void resize_objective(int n);

		evaluation_tag evaluate(); // evaluate each individual 
		virtual evaluation_tag evolve() { return evaluation_tag::Normal; }

		void reset(); // delete all individuals
		real mean(int oidx);	//mean value of the oidx-th objective
		real variance(int oidx, real mean); //variance of the oidx-th objective

		virtual void initialize(); //a uniformly distributed initialization by default
		template<typename Fun, typename Problem, typename... Args>
		void initialize(Fun fun, const Problem *pro, Args && ... args);

		std::map<real, size_t> nearest_neighbour(size_t idx, int k = 1);

		void reset_improved_flag();
		real rank(const solution_type &x);
		int number_PF() { return m_order.rbegin()->first; }
		int id() const { return m_id; }
		void resize(size_t size);
		bool& update_flag_best() { return m_best_updated; }
		bool& update_flag_worst() {	return m_worst_updated;	}
	protected:		
		bool update_best(const solution_type&);
		bool update_worst(const solution_type&);
	protected:
		size_t m_iter = 0;			// the current number of iterations
		int m_id;
		std::vector<std::unique_ptr<individual_type>> m_inds;
		std::vector<std::unique_ptr<solution_type>> m_best, m_worst;
		std::multimap<int, int> m_order;
		bool m_ordered = false, m_best_updated = false, m_worst_updated = false, m_best_monitor = true, m_worst_monitor = false;
	private:
		int m_max_id;
	};

	template<typename Individual>
	const std::vector<int> population<Individual>::order(int idx) {
		if (!m_ordered)
			sort();
		auto range_ = m_order.equal_range(idx);
		std::vector<int> inds;
		for (auto i = range_.first; i != range_.second; ++i)
			inds.push_back(i->second);
		return inds;
	}

	template<typename Individual>
	void population<Individual>::update_best() {
		if (!m_best_monitor||m_best_updated)
			return;
		m_best.clear();
		std::vector<bool> flag(m_inds.size(), true);

		for (size_t j = 0; j<m_inds.size(); j++) {
			for (size_t i = 0; i<m_inds.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (m_inds[j]->dominate(*m_inds[i])) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i < m_inds.size(); i++) {
			if (flag[i])
				m_best.push_back(std::make_unique<solution_type>(*m_inds[i]));
		}
		m_best_updated = true;
	}

	template<typename Individual>
	void population<Individual>::update_worst() {
		if (!m_worst_monitor||m_worst_updated)
			return;
		m_worst.clear();
		std::vector<bool> flag(m_inds.size(), true);

		for (size_t j = 0; j<m_inds.size(); j++) {
			for (size_t i = 0; i<m_inds.size(); i++) {
				if (i == j || !flag[j] || !flag[i]) continue;
				if (m_inds[i]->dominate(*m_inds[j])) {
					flag[i] = false;
				}
			}
		}
		for (size_t i = 0; i<m_inds.size(); i++) { 
			if (flag[i])
				m_worst.push_back(std::make_unique<solution_type>(*m_inds[i]));
		}
		m_worst_updated = true;
	}

	template<typename Individual>
	bool population<Individual>::update_best(const solution_type&x) {
		if (m_best.empty()) {
			m_best.push_back(std::make_unique<solution_type>(x));
			return true;
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
		if (!first) return true;
		//check equal case	
		for (auto i = m_best.begin(); i != m_best.end(); i++) {
			if (x.equal(**i) && !((*i)->same(x))) { //**i == x 
				m_best.push_back(std::make_unique<solution_type>(x));
				return true;
			}
		}
		//check non-dominated case	
		for (auto i = m_best.begin(); i != m_best.end(); i++) {
			if (!((*i)->nondominate(x))) return false;
		}
		m_best.push_back(std::make_unique<solution_type>(x));
		m_best_updated = true;
		return true;
	}

	template<typename Individual>
	bool population<Individual>::update_worst(const solution_type&x) {
		if (m_worst.empty()) {
			m_worst.push_back(std::make_unique<solution_type>(x));
			return true;
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
		if (!first) return true;
		//check equal case	
		for (auto i = m_worst.begin(); i != m_worst.end(); i++) {
			if (x.equal(**i) && !((*i)->same(x))) { //**i == x 
				m_worst.push_back(std::make_unique<solution_type>(x));
				return true;
			}
		}
		//check non-dominated case	
		for (auto i = m_worst.begin(); i != m_worst.end(); i++) {
			if (!((*i)->nondominate(x))) return false;
		}
		m_worst.push_back(std::make_unique<solution_type>(x));
		m_worst_updated = true;
		return true;
	}

	template<typename Individual>
	template<typename ... Args>
	population<Individual>::population(size_t n, Args&& ... args) : m_inds(n) {
		size_t num_obj = global::ms_global->m_problem->objective_size();
		size_t num_con = global::ms_global->m_problem->num_constraints();
		for (auto& i : m_inds)
			i = std::make_unique<Individual>(num_obj, num_con, std::forward<Args>(args)...);
	}

	template<typename Individual>
	population<Individual>::population(const population &rhs) : m_iter(rhs.m_iter), m_id(rhs.m_id),\
	m_best(rhs.m_best), m_worst(rhs.m_worst), m_order(rhs.m_order), m_ordered(rhs.m_ordered), m_best_updated(rhs.m_best_updated),\
	m_worst_updated(rhs.m_worst_updated), m_max_id(rhs.m_max_id) {
		for (size_t i = 0; i < rhs.size(); i++)	{
			m_inds.push_back(std::make_unique<Individual>(rhs[i]));
		}
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(const population &rhs) {
	    if (&rhs == this)
	        return *this;
		m_iter = rhs.m_iter;
		m_id = rhs.m_id;
		for (size_t i = 0; i < rhs.size(); i++)
			m_inds.push_back(std::make_unique<Individual>(rhs[i]));
		m_order = rhs.m_order;
		m_ordered = rhs.m_ordered;
		m_best_updated = rhs.m_best_updated;
		m_worst_updated = rhs.m_worst_updated;
		m_max_id = rhs.m_max_id;
		m_best_monitor = rhs.m_best_monitor;
		m_worst_monitor = rhs.m_worst_monitor;
		update_best();
		update_worst();
		return *this;
	}

	template<typename Individual>
	population<Individual>::population(population&&rhs) : m_iter(rhs.m_iter), m_id(rhs.m_id), m_inds(std::move(rhs.m_inds)), \
		m_best(std::move(rhs.m_best)), m_worst(std::move(rhs.m_worst)), m_order(std::move(rhs.m_order)), m_ordered(rhs.m_ordered),\
		m_best_updated(rhs.m_best_updated),	m_worst_updated(rhs.m_worst_updated), m_max_id(rhs.m_max_id) {
		
	}

	template<typename Individual>
	population<Individual>& population<Individual>::operator=(population &&rhs) {
        if (&rhs == this)
            return *this;
		m_iter = rhs.m_iter;
		m_id = rhs.m_id;
		m_inds = std::move(rhs.m_inds);
		m_best = std::move(rhs.m_best);
		m_worst = std::move(rhs.m_worst);
		m_order = std::move(rhs.m_order);
		m_ordered = rhs.m_ordered;
		m_best_updated = rhs.m_best_updated;
		m_worst_updated = rhs.m_worst_updated;
		m_max_id = rhs.m_max_id;
		m_best_monitor = rhs.m_best_monitor;
		m_worst_monitor = rhs.m_worst_monitor;
		return *this;
	}

	template<typename Individual>
	void population<Individual>::reset() {
		m_iter = 0;
		m_best.clear();
		m_worst.clear();
		m_inds.clear();
		m_order.clear();	
	}

	template<typename Individual>
	void population<Individual>::initialize( ) {	
		for (int i = 0; i < m_inds.size(); ++i) {
			m_inds[i]->initialize(i);
		}
		m_ordered = false;
		m_best_updated = m_worst_updated = false;
	}

	template<typename Individual>
	template<typename Fun, typename Problem, typename... Args>
	void  population<Individual>::initialize(Fun fun, const Problem* pro, Args&& ... args) {
		fun(m_inds, pro, std::forward<Args>(args)...);
	}

	template<typename Individual>
	void population<Individual>::sort() {
		const size_t N = m_inds.size();
		std::vector<std::vector<objective_type>*> data(N);
		for (size_t i = 0; i < N; ++i)
			data[i] = &(m_inds[i]->objective());
		std::vector<int> rank;
		//NS::fast_sort<objective_type>(data, rank, global::ms_global->m_problem->opt_mode());
		NS::filter_sort_p<objective_type>(data, rank, global::ms_global->m_problem->opt_mode(), 8);
		m_order.clear();
		for (size_t i = 0; i < N; i++) {
			m_order.insert(std::pair<int, int>(rank[i], m_inds[i]->id()));
			m_inds[i]->set_rank(rank[i]);
		}
		m_ordered = true;
	}

	template<typename Individual>
	std::map<real, size_t> population<Individual>::nearest_neighbour(size_t idx, int k) {
		//TODO return k nearest neighbour of individual idx together with the distance
		real Min_dis;
		size_t index;
		size_t count = 0;
		std::vector<real> dis(size());
		for (size_t i = 0; i < size(); ++i) {
			if (idx == i) {
				dis[i] = 0;
				continue;
			}
			dis[i] = m_inds[idx]->variable_distance(*m_inds[i]);
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
		
		std::map<real, size_t> result;
		result[Min_dis] = index;

		return result;
	}

	template<typename Individual>
	evaluation_tag population<Individual>::evaluate() {
		evaluation_tag tag = evaluation_tag::Normal;
		for (auto &i : m_inds) {
			tag = i->evaluate();
			if (tag != evaluation_tag::Normal) break;
		}
		return tag;
	}

	template<typename Individual>
	void population<Individual>::resize_objective(int n) {
		for (auto &i : m_inds)
			i->resize_objective(n);
	}

	template<typename Individual>
	void population<Individual>::reset_improved_flag() {
		for (auto &i : m_inds) {
			i->set_improved_flag(false);
		}
	}

	template<typename Individual>
	real population<Individual>::rank(const typename Individual::solution_type &x) {
		int i = 0;
		while (1) {
			auto l = m_order.lower_bound(i), u = m_order.upper_bound(i);
			while (l != u) {
				dominationship r = objective_compare(x.objective(), m_inds[l->second]->objective(), global::ms_global->m_problem->opt_mode());
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
		if (size > m_inds.size()) {
			for (auto i = m_inds.size(); i < size; i++) {
				m_inds.push_back(std::unique_ptr<Individual>(new Individual()));
			}
		}
		else if (size < m_inds.size()) {
			m_inds.resize(size);
		}
		
		m_ordered = m_best_updated = m_worst_updated = false;
	}
	
	template<typename Individual>
	void population<Individual>::add(Individual&& p) {
		m_inds.emplace_back(std::make_unique<Individual>(p));
		/* Set id */ 
		// TODO
	}

	template<typename Individual>
	void population<Individual>::remove(size_t idx) {
		if (idx >= m_inds.size()) return;
		m_inds.erase(m_inds.begin() + idx);
	}
}

#endif // !OFEC_POPULATION_H
