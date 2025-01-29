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
* class Swarm for TParticle swarm optimization
*
*
*********************************************************************************/
// Created: 21 Sep. 2011 by Changhe Li
// Updated: 12 Dec. 2014 by Changhe Li
// Updated  15 Mar. 2018 by Junchen Wang
// Updated: 1 Sep. 2018 by Changhe Li
// Updated: 25.Aug. 2019 Changhe Li

#ifndef OFEC_SWARM_H
#define OFEC_SWARM_H

#include "particle.h"
#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include <numeric>

namespace ofec {
	template <typename TParticle = Particle>
	class Swarm : public Population<TParticle> {
	protected:			
		Real m_accelerator1 = 1.496, m_accelerator2 = 1.496;		// accelerators
		Real m_weight = 0.7298;						// inertia weight		
		std::vector<std::vector<bool>> m_link;	// population topology
		bool m_flag_best_improved = false;

	public:
		Swarm() = default;
		Swarm(size_t size_pop, Environment *env);
		virtual void resize(size_t size_pop, Environment *env);
		void clear() override;
		void initPbest(Environment *env);
		void initVelocity(Environment *env, Random *rnd);
		void initVelocityMax(Environment *env, Random *rnd);
		void updateBest(Environment *env) override;
		void updateWorst(Environment *env) override;
		int evolve(Environment *env, Random *rnd) override;
		Real& weight() { return m_weight; }
		Real& accelerator1() { return m_accelerator1; }
		Real& accelerator2() { return m_accelerator2; }
		Real weight() const { return m_weight; }
		Real accelerator1() const { return m_accelerator1; }
		Real accelerator2() const { return m_accelerator2; }
		Real averageSpeed() const;
		bool bestImproved() const { return m_flag_best_improved; }
		virtual void setNeighborhood(Random *rnd);		// The adaptive random topology by default
		virtual Solution<>& neighborhoodBest(int idx, Environment *env);
	};
	
	template<typename TParticle>
	Swarm<TParticle>::Swarm(size_t size_pop, Environment *env) : 
		Population<TParticle>(size_pop, env, env->problem()->numberVariables()),
		m_link(size_pop, std::vector<bool>(size_pop, false)) {}

	template<typename TParticle>
	void Swarm<TParticle>::resize(size_t size_pop, Environment *env) {
		Population<TParticle>::resize(size_pop, env, env->problem()->numberVariables());
		m_link.resize(size_pop, std::vector<bool>(size_pop));
	}

	template<typename TParticle>
	void Swarm<TParticle>::clear() {
		Population<TParticle>::clear();
		m_link.clear();
		m_flag_best_improved = false;
	}

	template<typename TParticle>
	void Swarm<TParticle>::initPbest(Environment *env) {
		for (auto &i : this->m_individuals) {
			i->pbest() = *i;
		}
		updateBest(env);
	}

	template<typename TParticle>
	void Swarm<TParticle>::initVelocity(Environment *env, Random *rnd) {
		for (auto &i : this->m_individuals) {
			i->initVelocity(env, rnd);
		}
	}

	template<typename TParticle>
	void Swarm<TParticle>::initVelocityMax(Environment *env, Random *rnd) {
		for (auto &i : this->m_individuals) {
			i->initVelocityMax(env, rnd);
		}
	}

	template<typename TParticle>
	void Swarm<TParticle>::updateBest(Environment *env) {
		this->m_best_individual = nullptr;
		for (auto &ptr : this->m_individuals) {
			if (!this->m_best_individual ||
				ptr->pbest().dominate(this->m_best_individual->pbest(), env)
			) {
				this->m_best_individual = ptr.get();
			}
		}
	}

	template<typename TParticle>
	void Swarm<TParticle>::updateWorst(Environment *env) {
		this->m_worst_individual = nullptr;
		for (auto &ptr : this->m_individuals) {
			if (!this->m_worst_individual || ptr->pbest().dominate(this->m_worst_individual->pbest(), env)) {
				this->m_worst_individual = ptr.get();
			}
		}
	}

	template<typename TParticle>
	int Swarm<TParticle>::evolve(Environment *env, Random *rnd)	{
		int rf = kNormalEval;
		//generate a permutation of particle index
		std::vector<int> rindex(this->m_individuals.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		rnd->uniform.shuffle(rindex.begin(), rindex.end());

		this->setNeighborhood(rnd);

		m_flag_best_improved = false;
		for (int i = 0; i < this->m_individuals.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], env);

			this->m_individuals[rindex[i]]->nextVelocity(&x, m_weight, m_accelerator1, m_accelerator2, rnd);
			this->m_individuals[rindex[i]]->move();
			this->m_individuals[rindex[i]]->clampVelocity(env, rnd);

			rf = this->m_individuals[rindex[i]]->evaluate(env);

			if (this->m_individuals[rindex[i]]->dominate(this->m_individuals[rindex[i]]->pbest(), env)) {
				this->m_individuals[rindex[i]]->pbest() = *(this->m_individuals[rindex[i]]);
				this->m_individuals[rindex[i]]->setImproved(true);
				if (this->m_individuals[rindex[i]]->pbest().dominate(this->m_best_individual->pbest(), env)) {
					m_flag_best_improved = true;
				}
			}
			else {
				this->m_individuals[rindex[i]]->setImproved(false);
			}

			if (rf & kTerminate) {
				break;
			}
		}
		this->m_iteration++;
		return rf;
	}

	template<typename TParticle>
	Real Swarm<TParticle>::averageSpeed() const {
		Real avg = 0;		
		for (int i = 0; i < this->m_individuals.size(); ++i) {
			avg += this->m_individuals[i]->speed();
		}
		return avg > 0 ? avg / this->m_individuals.size() : 0;
	}

	template<typename TParticle>
	Solution<>& Swarm<TParticle>::neighborhoodBest(int idx, Environment *env) {
		int l = idx;
		for (int i = 0; i < this->m_individuals.size();++i) {
			if (m_link[idx][i] && this->m_individuals[i]->pbest().dominate( this->m_individuals[l]->pbest(), env)) {
				l = i;
			}
		}
		return this->m_individuals[l]->pbest();
	}

	template<typename TParticle>
	void Swarm<TParticle>::setNeighborhood(Random *rnd) {
		if (m_link.size() != this->m_individuals.size()) {
			m_link.assign(this->m_individuals.size(), std::vector<bool>(this->m_individuals.size()));
			m_flag_best_improved = false;
		}
		if (!m_flag_best_improved) {
			Real p = 1 - pow(1 - 1. / this->m_individuals.size(), 3);
			for (int i = 0; i<this->m_individuals.size(); i++) {
				for (int k = 0; k<this->m_individuals.size(); k++) {
					if (k == i || rnd->uniform.next() < p) {
						m_link[i][k] = true; //probabilistic method
					}
					else {
						m_link[i][k] = true;
					}
				}
			}
		}
	}

}
#endif // !OFEC_SWARM_H


