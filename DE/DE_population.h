#pragma once
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
* class DE_population manages a set of DE_individuals.
*
*********************************************************************************/

/*Storn, R. and Price, K. (1997), "Differential Evolution - A Simple and Efficient Heuristic for Global Optimization over Continuous Spaces",
Journal of Global Optimization, 11, pp. 341-359*/
#ifndef OFEC_DE_POPULATION_H
#define OFEC_DE_POPULATION_H
#include "../core/algorithm/population.h"

namespace OFEC {

	template< typename TypeDEIndi >
	class DE_population : public population<TypeDEIndi> {
	protected:
		double  m_F, m_CR;
		DEMutationStratgy m_mutStrategy;
	public:
		virtual ~DE_population() {}

		DE_population() = default;
		DE_population(int n);
		DE_population(const DE_population &rhs);
		DE_population(DE_population&&rhs);

		DE_population& operator=(const DE_population &rhs);
		DE_population& operator=(DE_population&&rhs);

		void setMutationStrategy(DEMutationStratgy rS);
		virtual void mutate(const int idx);
		void setParmeter(double cr, double f);
		void defaultParameter();
		virtual void reInitialize(bool clearOldBest, bool mode);
		void mutate(int idx, const std::vector<int>&var);
	protected:
		virtual void selectInNeighb(int number, std::vector<int>&, std::vector<Solution<CodeVReal>*>&);
		virtual void select(int base, int number, std::vector<int>& result);
		evaluation_tag evolve();

	};

	template< typename TypeDEIndi >
	DE_population<TypeDEIndi>::DE_population(int n) : population(n) {
		defaultParameter();
	}
	template< typename TypeDEIndi >
	DE_population<TypeDEIndi>::DE_population(const DE_population &rhs) : population(rhs), m_F(rhs.m_F), m_CR(rhs.m_CR), m_mutStrategy(rhs.m_mutStrategy) {

	}
	template< typename TypeDEIndi >
	DE_population<TypeDEIndi>::DE_population(DE_population&&rhs) : population(std::move(rhs)), m_F(std::move(rhs.m_F)), m_CR(std::move(rhs.m_CR)), \
		m_mutStrategy(std::move(rhs.m_mutStrategy)) {
		
	}
	
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::setMutationStrategy(DEMutationStratgy rS) {
		m_mutStrategy = rS;
	}
	template< typename TypeDEIndi >
	DE_population<TypeDEIndi> & DE_population<TypeDEIndi>::operator=(const DE_population & rhs) {
		if (this == &rhs) return *this;
		population::operator=(rhs);
		m_CR = rhs.m_CR;
		m_F = rhs.m_F;
		m_mutStrategy = rhs.m_mutStrategy;
		return *this;
	}
	template< typename TypeDEIndi >
	DE_population<TypeDEIndi> & DE_population<TypeDEIndi>::operator=(DE_population&& rhs) {
		if (this == &rhs) return *this;
		population::operator=(std::move(rhs));
		m_CR = std::move(rhs.m_CR);
		m_F = std::move(rhs.m_F);
		m_mutStrategy = std::move(rhs.m_mutStrategy);
		return *this;
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::mutate(const int idx) {
		std::vector<int> ridx;
		switch (m_mutStrategy) {
		case DE_rand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_best_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_targetToBest_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_best[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_best_2:
			select(idx, 4, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self());
			break;
		case DE_rand_2:
			select(idx, 5, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self(), &this->m_pop[ridx[4]]->self());
			break;
		case DE_randToBest_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_targetToRand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		}
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::mutate(int idx, const std::vector<int>&var) {
		std::vector<int> ridx;
		switch (m_mutStrategy) {
		case DE_rand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_best_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_targetToBest_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[idx]->self(), &this->m_best[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_best_2:
			select(idx, 4, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self());
			break;
		case DE_rand_2:
			select(idx, 5, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self(), &this->m_pop[ridx[4]]->self());
			break;
		case DE_randToBest_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_targetToRand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		}
	}
	template< typename TypeDEIndi >
	evaluation_tag DE_population<TypeDEIndi>::evolve() {
		if (this->m_pop.size() < 5) {
			throw myException("the population size cannot be smaller than 5@DE_population<ED,TypeDEIndi>::evolve()");
		}
		evaluation_tag tag = evaluation_tag::Normal;

		for (int i = 0; i < this->m_pop.size(); i++) {
			mutate(i);
			this->m_pop[i]->recombine(m_CR);
		}

		this->updateIDnIndex();
		for (int i = 0; i < this->m_pop.size(); i++) {
			tag = this->m_pop[i]->select();
			this->update_archive(this->m_pop[i]->self());
			if (tag != evaluation_tag::Normal) break;

		}

		if (tag == evaluation_tag::Normal) {
			//this->m_center = *this->m_best[0];
			//this->updateCurRadius();
			this->m_iter++;
		}
		return tag;
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::setParmeter(const double cr, const double f) {
		m_CR = cr;
		m_F = f;
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::defaultParameter() {
		m_CR = 0.6;
		m_F = 0.5;
		m_mutStrategy = DE_best_1;
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::reInitialize(bool clearOldBest, bool mode) {
		for (int i = 0; i < this->m_pop.size(); i++) {
			this->m_pop[i]->initialize(mode);
			//this->m_pop[i]->m_flag = true;
		}
		if (this->terminating()) return;
		this->update_best();
		if (clearOldBest) this->m_best.clear();
		for (unsigned i = 0; i < this->m_best.size(); i++) {
			this->update_archive(this->m_best[i].self());
		}
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::select(int base, int number, std::vector<int>& result) {
		std::std::vector<int> candidate;
		for (int i = 0; i < this->m_pop.size(); i++) {
			if (base != i) candidate.push_back(i);
		}
		result.resize(number);
		for (int i = 0; i < number; ++i) {
			int idx = global::ms_global->getRandInt(0, candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}
	template< typename TypeDEIndi >
	void DE_population<TypeDEIndi>::selectInNeighb(int number, std::vector<int>& candidate, std::vector<solution_type*>& result) {
		for (int i = 0; i < number; ++i) {
			int idx = global::ms_global->getRandInt(0, candidate.size() - i);
			result[i] = &(m_pop[candidate[idx]]->self());
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}
}
#endif // !OFEC_DE_POPULATION_H


