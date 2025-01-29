/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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
* class Population manages a set of Solutions.
*
*********************************************************************************/

#ifndef OFEC_POPULATION_H
#define OFEC_POPULATION_H

#include "../environment/environment.h"
#include <algorithm>

namespace ofec {
	template<typename TPopulation>
	class MultiPopulation;

	template<typename TIndividual>
	class Population {
	public:
		template<typename TPopulation>
		friend class MultiPopulation;
		using IndividualType = TIndividual;
		using SolutionType = typename IndividualType::SolutionType;
		using IteratorType = typename std::vector<std::unique_ptr<IndividualType>>::iterator;
		using CIteratorType = typename std::vector<std::unique_ptr<IndividualType>>::const_iterator;

	protected:
		int m_id = -1;
		size_t m_iteration = 0;			// the current number of iterations
		bool m_active = true;
		std::vector<std::unique_ptr<IndividualType>> m_individuals;
		TIndividual *m_best_individual, *m_worst_individual = nullptr;

	public:
		virtual ~Population() = default;
		Population() = default;
		template<typename ... Args>
		Population(size_t n, Environment *env, Args&& ...args) : m_individuals(n) {
			size_t number_objectives = env->problem()->numberObjectives();
			size_t num_cons = env->problem()->numberConstraints();
			for (auto &i : m_individuals) {
				i = std::make_unique<IndividualType>(number_objectives, num_cons, std::forward<Args>(args)...);
			}
		}
		Population(const Population &rhs) : 
			m_id(rhs.m_id),
			m_iteration(rhs.m_iteration),
			m_active(rhs.m_active)
		{
			for (size_t i = 0; i < rhs.size(); i++) {
				m_individuals.emplace_back(new IndividualType(rhs[i]));
			}
		}

		Population(Population &&rhs) = default;

		Population& operator=(const Population &rhs) {
			if (&rhs == this)
				return *this;
			m_id = rhs.m_id;
			m_iteration = rhs.m_iteration;
			m_active = rhs.m_active;
			m_individuals.clear();
			for (size_t i = 0; i < rhs.size(); i++)
				m_individuals.emplace_back(new IndividualType(rhs[i]));
			return *this;
		}

		Population& operator=(Population &&rhs) = default;

		TIndividual* best() const { return m_best_individual; }
		TIndividual* best(Environment *env) {
			updateBest(env);
			return m_best_individual;
		}
		virtual void updateBest(Environment *env) {
			m_best_individual = nullptr;
			for (auto &ptr : m_individuals) {
				if (!m_best_individual || ptr->dominate(*m_best_individual, env)) {
					m_best_individual = ptr.get();
				}
			}
		}

		TIndividual* worst() const { return m_worst_individual; }
		TIndividual* worst(Environment *env) {
			updateWorst(env);
			return m_worst_individual;
		}
		virtual void updateWorst(Environment *env) {
			m_worst_individual = nullptr;
			for (auto &ptr : m_individuals) {
				if (!m_worst_individual || m_worst_individual->dominate( *ptr, env)) {
					m_worst_individual = ptr.get();
				}
			}
		}

		size_t size() const noexcept { return m_individuals.size(); }
		const IndividualType& operator[](size_t i) const { return *m_individuals[i]; }
		IndividualType& operator[](size_t i) { return *m_individuals[i]; }
		const IndividualType& at(size_t i) const { return *m_individuals[i]; }
		IndividualType& at(size_t i) { return *m_individuals[i]; }
		IteratorType begin() { return m_individuals.begin(); }
		IteratorType end() { return m_individuals.end(); }
		CIteratorType begin() const { return m_individuals.begin(); }
		CIteratorType end() const { return m_individuals.end(); }
		IndividualType& front() { return *m_individuals.front(); }
		IndividualType& back() { return *m_individuals.back(); }
		const IndividualType& front() const { return *m_individuals.front(); }
		const IndividualType& back() const { return *m_individuals.back(); }

		virtual void append(IndividualType &&ind) {
			m_individuals.emplace_back(new IndividualType(std::forward<IndividualType>(ind)));
		}

		virtual void append(std::unique_ptr<IndividualType> &ind) {
			m_individuals.emplace_back(ind.release());
		}

		virtual void append(const IndividualType &ind) {
			m_individuals.emplace_back(new IndividualType(ind));
		}

		virtual void append(Population<IndividualType> &pop) {
			size_t size_before = m_individuals.size();
			m_individuals.resize(size_before + pop.m_individuals.size());
			for (size_t i = size_before; i < m_individuals.size(); i++) {
				m_individuals[i].reset(pop.m_individuals[i - size_before].release());
			}
		}

		virtual IteratorType remove(IteratorType iter_ind) {
			return m_individuals.erase(iter_ind);
		}

		template<typename ... Args>
		void resize(size_t size, Environment *env, Args&& ...args) {
			if (m_individuals.size() > size)
				m_individuals.resize(size);
			else if (m_individuals.size() < size) {
				size_t number_objectives = env->problem()->numberObjectives();
				size_t num_cons = env->problem()->numberConstraints();
				for (size_t i = m_individuals.size(); i < size; i++)
					m_individuals.emplace_back(new IndividualType(number_objectives, num_cons, std::forward<Args>(args)...));
			}
		}

		virtual void clear() {
			m_iteration = 0;
			m_active = true;
			m_individuals.clear();
		}

		virtual void initialize(Environment *env, Random *rnd) {
			m_iteration = 0;
			m_active = true;
			for (int i = 0; i < m_individuals.size(); ++i) {
				m_individuals[i]->initialize(env, rnd);
			}
		}

		virtual int evaluate(Environment *env) {
			int tag = kNormalEval;
			for (auto &i : m_individuals) {
				tag = i->evaluate(env);
				if (tag != kNormalEval) break;
			}
			return tag;
		}

		virtual int evolve(Environment *env, Random *rnd) { return kNormalEval; }

		virtual void validate(Environment *env, Validation mode, Random *rnd = nullptr) {
			for (auto &i : m_individuals) {
				i->validate(env, mode, rnd);
			}
		}

		int id() const { return m_id; }
		virtual void setId(int id) { m_id = id; }
		size_t iteration() const { return m_iteration; }
		//size_t& iteration() { return m_iteration; }
		void resetIteration() { m_iteration = 0; }
		void increaseIteration() { m_iteration++; }
		virtual bool isActive() { return m_active; }
		void setActive(bool value) { m_active = value; }
		virtual void handleEvaluationTag(int tag) {}
	};
}

#endif // !OFEC_POPULATION_H
