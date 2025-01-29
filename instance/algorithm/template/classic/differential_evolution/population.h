/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
* class DEpopulation manages a set of DESolutions.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

/*Storn, R. and Price, K. (1997), "Differential Evolution - A Simple and Efficient Heuristic for Global Optimization over Continuous Spaces",
Journal of Global Optimization, 11, pp. 341-359*/

#ifndef OFEC_DE_POPULATION_H
#define OFEC_DE_POPULATION_H

#include "individual.h"
#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/problem/continuous/continuous.h"

namespace ofec {
	template<typename TIndividual = IndividualDE>
	class PopulationDE : public Population<TIndividual> {
	protected:
		Real m_scaling_factor = 0.5, m_crossover_rate = 0.6;
		de::MutateStrategy m_mutation_strategy = de::MutateStrategy::kRand1;
		de::RecombineStrategy m_recombine_strategy = de::RecombineStrategy::kBinomial;

	public:
		using Population<TIndividual>::m_individuals;
		using Population<TIndividual>::m_iteration;

		PopulationDE() = default;
		PopulationDE(size_t size_pop, Environment *env);
		PopulationDE(const PopulationDE& rhs);
		PopulationDE(PopulationDE&& rhs) noexcept;
		virtual void resize(size_t size_pop, Environment *env);

		PopulationDE& operator=(const PopulationDE& rhs);
		PopulationDE& operator=(PopulationDE&& rhs) noexcept;

		Real& scalingFactor() { return m_scaling_factor; }
		Real& crossoverRate() { return m_crossover_rate; }
		de::MutateStrategy& mutationStrategy() { return m_mutation_strategy; }
		de::RecombineStrategy& recombineStrategy() { return m_recombine_strategy; }
		void setParameter(const Real CR, const Real F);
		Real scalingFactor() const { return m_scaling_factor; }
		Real crossoverRate() const { return m_crossover_rate; }
		de::MutateStrategy mutationStrategy() const { return m_mutation_strategy; }
		de::RecombineStrategy recombineStrategy() const { return m_recombine_strategy; }

		int evolve(Environment *env, Random *rnd) override;
		virtual void mutate(int idx, Random *rnd, Environment *env);
		void mutate(int idx, const std::vector<int>& var, Random *rnd, Environment *env);
		void recombine(int idx, Random *rnd, Environment *env);
		virtual void select(int base, int number, std::vector<size_t> &result, Random *rnd);
		virtual void selectInCandidates(
			int number, std::vector<size_t> candidates, std::vector<size_t> &result, Random *rnd
		);
		virtual void selectInCandidates(
			int base, 
			int number, std::vector<size_t> candidates, std::vector<size_t> &result, Random *rnd
		);
	};

	template<typename TIndividual>
	PopulationDE<TIndividual>::PopulationDE(size_t size_pop, Environment *env) :
		Population<TIndividual>(size_pop, env, env->problem()->numberVariables()) {}

	template<typename TIndividual>
	PopulationDE<TIndividual>::PopulationDE(const PopulationDE& rhs) :
		Population<TIndividual>(rhs),
		m_scaling_factor(rhs.m_scaling_factor),
		m_crossover_rate(rhs.m_crossover_rate),
		m_mutation_strategy(rhs.m_mutation_strategy),
		m_recombine_strategy(rhs.m_recombine_strategy) {}

	template<typename TIndividual>
	PopulationDE<TIndividual>::PopulationDE(PopulationDE&& rhs) noexcept :
		Population<TIndividual>(std::move(rhs)),
		m_scaling_factor(rhs.m_scaling_factor),
		m_crossover_rate(rhs.m_crossover_rate),
		m_mutation_strategy(rhs.m_mutation_strategy),
		m_recombine_strategy(rhs.m_recombine_strategy) {}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::resize(size_t size_pop, Environment *env) {
		Population<TIndividual>::resize(size_pop, env, env->problem()->numberVariables());
	}

	template<typename TIndividual>
	PopulationDE<TIndividual>& PopulationDE<TIndividual>::operator=(const PopulationDE& rhs) {
		if (this == &rhs) return *this;
		Population<TIndividual>::operator=(rhs);
		m_crossover_rate = rhs.m_crossover_rate;
		m_scaling_factor = rhs.m_scaling_factor;
		m_mutation_strategy = rhs.m_mutation_strategy;
		m_recombine_strategy = rhs.m_recombine_strategy;
		return *this;
	}

	template<typename TIndividual>
	PopulationDE<TIndividual>& PopulationDE<TIndividual>::operator=(PopulationDE&& rhs) noexcept {
		if (this == &rhs) return *this;
		Population<TIndividual>::operator=(std::move(rhs));
		m_crossover_rate = rhs.m_crossover_rate;
		m_scaling_factor = rhs.m_scaling_factor;
		m_mutation_strategy = rhs.m_mutation_strategy;
		m_recombine_strategy = rhs.m_recombine_strategy;
		return *this;
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::mutate(const int idx, Random *rnd, Environment *env) {
		std::vector<size_t> ridx;
		switch (m_mutation_strategy) {
		case de::MutateStrategy::kRand1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get(), env);
			break;
		case de::MutateStrategy::kBest1:
			select(idx, 2, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, this->m_best_individual, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), env);
			break;
		case de::MutateStrategy::kCurrentToBest1:
			select(idx, 2, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, m_individuals[idx].get(), this->m_best_individual, m_individuals[idx].get(), env, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get());
			break;
		case de::MutateStrategy::kBest2:
			select(idx, 4, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, this->m_best_individual, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), env, m_individuals[ridx[2]].get(), m_individuals[ridx[3]].get());
			break;
		case de::MutateStrategy::kRand2:
			select(idx, 5, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get(), env, m_individuals[ridx[3]].get(), m_individuals[ridx[4]].get());
			break;
		case de::MutateStrategy::kRandToBest1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, m_individuals[ridx[0]].get(), this->m_best_individual, m_individuals[ridx[0]].get(), env, m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get());
			break;
		case de::MutateStrategy::kCurrentToRand1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, m_individuals[idx].get(), m_individuals[ridx[0]].get(), m_individuals[idx].get(), env, m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get());
			break;
		}
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::recombine(int idx, Random *rnd, Environment *env) {
		m_individuals[idx]->recombine(m_crossover_rate, m_recombine_strategy, rnd, env);
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::mutate(int idx, const std::vector<int>& var, Random *rnd, Environment *env) {
		std::vector<size_t> ridx;
		switch (m_mutation_strategy) {
		case de::MutateStrategy::kRand1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get(), env);
			break;
		case de::MutateStrategy::kBest1:
			select(idx, 2, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, this->m_best_individual, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), env);
			break;
		case de::MutateStrategy::kCurrentToBest1:
			select(idx, 2, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, m_individuals[idx].get(), this->m_best_individual, m_individuals[idx].get(), env, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get());
			break;
		case de::MutateStrategy::kBest2:
			select(idx, 4, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, this->m_best_individual, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), env, m_individuals[ridx[2]].get(), m_individuals[ridx[3]].get());
			break;
		case de::MutateStrategy::kRand2:
			select(idx, 5, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, m_individuals[ridx[0]].get(), m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get(), env, m_individuals[ridx[3]].get(), m_individuals[ridx[4]].get());
			break;
		case de::MutateStrategy::kRandToBest1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, m_individuals[ridx[0]].get(), this->m_best_individual, m_individuals[ridx[0]].get(), env, m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get());
			break;
		case de::MutateStrategy::kCurrentToRand1:
			select(idx, 3, ridx, rnd);
			m_individuals[idx]->mutate(m_scaling_factor, var, m_individuals[idx].get(), m_individuals[ridx[0]].get(), m_individuals[idx].get(), env, m_individuals[ridx[1]].get(), m_individuals[ridx[2]].get());
			break;
		}
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::setParameter(const Real CR, const Real F) {
		m_crossover_rate = CR;
		m_scaling_factor = F;
	}

	template<typename TIndividual>
	int PopulationDE<TIndividual>::evolve(Environment *env, Random *rnd) {
		if (m_individuals.size() < 5) {
			throw Exception("the population size cannot be smaller than 5@PopulationDE<TIndividual>::evolve()");
		}
		int tag = kNormalEval;
		if (m_mutation_strategy == de::MutateStrategy::kBest1 ||
			m_mutation_strategy == de::MutateStrategy::kCurrentToBest1 ||
			m_mutation_strategy == de::MutateStrategy::kBest2 ||
			m_mutation_strategy == de::MutateStrategy::kRandToBest1)
			this->updateBest(env);
		for (size_t i = 0; i < m_individuals.size(); ++i) {
			mutate(i, rnd, env);
			recombine(i, rnd, env);
			tag = m_individuals[i]->select(env);
			if (!(tag & kNormalEval)) return tag;
		}
		if (tag & kNormalEval)
			m_iteration++;
		return tag;
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::select(int base, int number, std::vector<size_t>& result, Random *rnd) {
		std::vector<size_t> candidates;
		for (size_t i = 0; i < m_individuals.size(); ++i) {
			if (base != i) {
				candidates.push_back(i);
			}
		}
		selectInCandidates(number, candidates, result, rnd);
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::selectInCandidates(
		int number, std::vector<size_t> candidates, std::vector<size_t> &result, Random *rnd
	) {
		if (result.size() != number) {
			result.resize(number);
		}
		for (size_t i = 0; i < number; ++i) {
			size_t idx = rnd->uniform.nextNonStd<size_t>(0, candidates.size() - i);
			result[i] = candidates[idx];
			if (idx != candidates.size() - (i + 1)) {
				candidates[idx] = candidates[candidates.size() - (i + 1)];
			}
		}
	}

	template<typename TIndividual>
	void PopulationDE<TIndividual>::selectInCandidates(
		int base, 
		int number, std::vector<size_t> candidates, std::vector<size_t> &result, Random *rnd
	) {
		std::vector<size_t> candidates_;
		for (size_t i : candidates) {
			if (base != i) {
				candidates_.push_back(i);
			}
		}
		selectInCandidates(number, candidates_, result, rnd);
	}
}
#endif // !OFEC_DE_POPULATION_H
