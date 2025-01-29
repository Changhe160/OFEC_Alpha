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
* class MultiPopulation manages a set of populations with the same type of Solutions.
*
*********************************************************************************/

#ifndef OFEC_MULTI_POPULATION_H
#define OFEC_MULTI_POPULATION_H

#include <vector>
#include <memory>
#include "../definition.h"

namespace ofec {
	class Environment;
	struct Random;
	template<typename TPopulation>
	class MultiPopulation {
	public:
		using PopulationType = TPopulation;
		using IteratorType = typename std::vector<std::unique_ptr<TPopulation>>::iterator;
		using CIteratorType = typename std::vector<std::unique_ptr<TPopulation>>::const_iterator;
	protected:
		std::vector<std::unique_ptr<TPopulation>> m_populations;
		int m_maxsize = -1;		// the maximum size of each sub-population
	public:
		MultiPopulation() = default;
		MultiPopulation(int n) : m_populations(n) {}
		template<typename ... Args>
		MultiPopulation(size_t n, size_t subsize, Environment *env, Args&& ...args) : m_populations(n) {
			for (auto &i : m_populations) {
				i.reset(new TPopulation(subsize, env, std::forward<Args>(args)...));
			}
		}
		virtual ~MultiPopulation() = default;

		/* Member access methods */
		size_t size() const { return m_populations.size(); }
		TPopulation& operator[](size_t i) { return *m_populations[i]; }
		const TPopulation& operator[](size_t i) const { return *m_populations[i]; }
		TPopulation& at(size_t i) { return *m_populations[i]; }
		const TPopulation& at(size_t i) const { return *m_populations[i]; }
		IteratorType begin() { return m_populations.begin(); }
		IteratorType end() { return m_populations.end(); }
		CIteratorType begin() const { return m_populations.begin(); }
		CIteratorType end() const { return m_populations.end(); }
		TPopulation& front() { return *m_populations.front(); }
		const TPopulation& front() const { return *m_populations.front(); }
		TPopulation& back() { return *m_populations.back(); }
		const TPopulation& back() const { return *m_populations.back(); }
		std::vector<std::unique_ptr<TPopulation>>& getPops() { return m_populations; }

		bool isActive() const {
			for (size_t k = 0; k < m_populations.size(); k++) {
				if (m_populations[k]->isActive()) {
					return true;
				}
			}
			return false;
		}

		void append(std::unique_ptr<TPopulation> &p) {
			m_populations.emplace_back(p.release());
			/* Set id */
			// TODO
		}

		void append(TPopulation &p) {
			m_populations.emplace_back(new TPopulation(p));
			/* Set id */
			// TODO
		}

		void append(TPopulation &&p) {
			m_populations.emplace_back(new TPopulation(std::forward<TPopulation>(p)));
			/* Set id */
			// TODO
		}

		IteratorType remove(IteratorType iter) {
			return m_populations.erase(iter);
		}

		void popBack() {
			m_populations.pop_back();
		}

		void clear() {
			m_populations.clear();
		}

		virtual IteratorType merge(IteratorType iter1, IteratorType iter2) {
			size_t pre_size = (*iter1)->size();
			(*iter1)->m_individuals.resize(pre_size + (*iter2)->size());
			for (size_t i = 0; i < (*iter2)->size(); i++) {
				std::swap((*iter1)->m_individuals[pre_size + i], (*iter2)->m_individuals[i]);
			}
			return m_populations.erase(iter2);
		}

		void resize(size_t N) {
			m_populations.resize(N);
			for (size_t k = 0; k < N; k++) {
				if (!m_populations[k]) {
					m_populations[k].reset(new TPopulation);
				}
			}
		}

		template<typename ... Args>
		void resize(size_t num_subpops, size_t subpop_size, Environment *env, Args&& ...args) {
			m_populations.resize(num_subpops);
			for (size_t k = 0; k < num_subpops; k++) {
				m_populations[k].reset(new TPopulation(subpop_size, env, std::forward<Args>(args)...));
			}
		}

		virtual void handleEvaluationTag(int& tag) {}

		virtual int evolve(Environment *env, Random *rnd) {
			int rf = EvaluationTag::kNormalEval;
			for (auto& subpop : m_populations) {
				if (!subpop->isActive()) continue;
				rf = subpop->evolve(env, rnd);
				if (!(rf & EvaluationTag::kNormalEval)) {
					this->handleEvaluationTag(rf);
					return rf;
				}
			}
			return rf;
		}

		void moveTo(MultiPopulation &pops) {
			pops.m_populations.clear();
			pops.m_populations.resize(m_populations.size());
			for (size_t k = 0; k < m_populations.size(); ++k) {
				pops.m_populations[k].reset(m_populations[k].release());
			}
			m_populations.clear();
		}
	};
}
#endif // !OFEC_MULTI_POPULATION_H
