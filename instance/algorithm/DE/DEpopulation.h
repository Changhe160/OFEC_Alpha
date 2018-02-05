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
* class DEpopulation manages a set of DE_individuals.
*
*********************************************************************************/

/*Storn, R. and Price, K. (1997), "Differential Evolution - A Simple and Efficient Heuristic for Global Optimization over Continuous Spaces",
Journal of Global Optimization, 11, pp. 341-359*/
#ifndef OFEC_DEPOPULATION_H
#define OFEC_DEPOPULATION_H
#include "../../../core/algorithm/population.h"

namespace OFEC {

	template< typename TypeDEIndi >
	class DEpopulation : public population<TypeDEIndi> {
	protected:
		double  m_F, m_CR;
		enum DEMutationStratgy { DE_rand_1, DE_best_1, DE_targetToBest_1, DE_best_2, DE_rand_2, DE_randToBest_1, DE_targetToRand_1 };
		DEMutationStratgy m_mutStrategy;
	public:
		using population_type = population<TypeDEIndi>;

		DEpopulation() = default;
		template<typename ... Args>
		DEpopulation(size_t no, Args&& ... args);
		DEpopulation(const DEpopulation &rhs);
		DEpopulation(DEpopulation&&rhs);

		DEpopulation& operator=(const DEpopulation &rhs);
		DEpopulation& operator=(DEpopulation&&rhs);

		void setMutationStrategy(DEMutationStratgy rS);
		virtual void mutate(const int idx);
		void setParmeter(double cr, double f);
		void defaultParameter();
		virtual void reInitialize();
		void mutate(int idx, const std::vector<int>&var);
	protected:
		virtual void selectInNeighb(int number, std::vector<int>&, std::vector<int>&);
		virtual void select(int base, int number, std::vector<int>& result);
		evaluation_tag evolve();

	};

	template< typename TypeDEIndi>
	template<typename ... Args>
	DEpopulation<TypeDEIndi>::DEpopulation(size_t no, Args&& ... args) : population_type(no, std::forward<Args>(args)...) {
		defaultParameter();
	}
	template< typename TypeDEIndi >
	DEpopulation<TypeDEIndi>::DEpopulation(const DEpopulation &rhs) : population_type(rhs), m_F(rhs.m_F), m_CR(rhs.m_CR), m_mutStrategy(rhs.m_mutStrategy) {

	}
	template< typename TypeDEIndi >
	DEpopulation<TypeDEIndi>::DEpopulation(DEpopulation&&rhs) : population_type(std::move(rhs)), m_F(std::move(rhs.m_F)), m_CR(std::move(rhs.m_CR)), \
		m_mutStrategy(std::move(rhs.m_mutStrategy)) {
		
	}
	
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::setMutationStrategy(DEMutationStratgy rS) {
		m_mutStrategy = rS;
	}
	template< typename TypeDEIndi >
	DEpopulation<TypeDEIndi> & DEpopulation<TypeDEIndi>::operator=(const DEpopulation & rhs) {
		if (this == &rhs) return *this;
		population::operator=(rhs);
		m_CR = rhs.m_CR;
		m_F = rhs.m_F;
		m_mutStrategy = rhs.m_mutStrategy;
		return *this;
	}
	template< typename TypeDEIndi >
	DEpopulation<TypeDEIndi> & DEpopulation<TypeDEIndi>::operator=(DEpopulation&& rhs) {
		if (this == &rhs) return *this;
		population::operator=(std::move(rhs));
		m_CR = rhs.m_CR;
		m_F = rhs.m_F;
		m_mutStrategy = std::move(rhs.m_mutStrategy);
		return *this;
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::mutate(const int idx) {
		std::vector<int> ridx;
		switch (m_mutStrategy) {
		case DE_rand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_best_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_targetToBest_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_arc[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_best_2:
			select(idx, 4, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self());
			break;
		case DE_rand_2:
			select(idx, 5, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self(), &this->m_pop[ridx[4]]->self());
			break;
		case DE_randToBest_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[ridx[0]]->self(), &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_targetToRand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		}
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::mutate(int idx, const std::vector<int>&var) {
		std::vector<int> ridx;
		switch (m_mutStrategy) {
		case DE_rand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_best_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_targetToBest_1:
			select(idx, 2, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[idx]->self(), &this->m_arc[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
			break;
		case DE_best_2:
			select(idx, 4, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self());
			break;
		case DE_rand_2:
			select(idx, 5, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self(), &this->m_pop[ridx[4]]->self());
			break;
		case DE_randToBest_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[ridx[0]]->self(), &this->m_arc[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		case DE_targetToRand_1:
			select(idx, 3, ridx);
			this->m_pop[idx]->mutate(m_F, var, &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
			break;
		}
	}
	template< typename TypeDEIndi >
	evaluation_tag DEpopulation<TypeDEIndi>::evolve() {
		if (this->m_pop.size() < 5) {
			throw myexcept("the population size cannot be smaller than 5@DEpopulation<TypeDEIndi>::evolve()");
		}
		evaluation_tag tag = evaluation_tag::Normal;

		for (int i = 0; i < this->m_pop.size(); ++i) {
			mutate(i);
			this->m_pop[i]->recombine(m_CR);
		}

		//this->updateIDnIndex();
		for (int i = 0; i < this->m_pop.size(); ++i) {
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
	void DEpopulation<TypeDEIndi>::setParmeter(const double cr, const double f) {
		m_CR = cr;
		m_F = f;
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::defaultParameter() {
		m_CR = 0.6;
		m_F = 0.5;
		m_mutStrategy = DE_best_1;
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::reInitialize() {
		for (int i = 0; i < this->m_pop.size(); ++i) {
			this->m_pop[i]->initialize(i);
			//this->m_pop[i]->m_flag = true;
		}
		if (this->terminating()) return;
		this->update_best();
		
		for (size_t i = 0; i < this->m_best.size(); ++i) {
			this->update_archive(this->m_best[i]->self());
		}
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::select(int base, int number, std::vector<int>& result) {
		std::vector<int> candidate;
		for (int i = 0; i < this->m_pop.size(); ++i) {
			if (base != i) candidate.push_back(i);
		}
		result.resize(number);
		for (int i = 0; i < number; ++i) {
			int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}
	template< typename TypeDEIndi >
	void DEpopulation<TypeDEIndi>::selectInNeighb(int number, std::vector<int>& candidate, std::vector<int>& result) {
		for (int i = 0; i < number; ++i) {
			int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}
}
#endif // !OFEC_DEPOPULATION_H


