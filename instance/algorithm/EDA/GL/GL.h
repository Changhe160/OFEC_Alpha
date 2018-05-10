/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li 
* Email: changhe.lw@google.com  
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Framework of genetic learning (GL) algorithm
*
*********************************************************************************/

#ifndef GENETIC_LEARNING_H
#define GENETIC_LEARNING_H

#include "adaptor.h"
#include <deque>
#include "../../../../utility/typevar/typevar.h"

namespace OFEC {
	template<typename Population>
	class GL :public Population {
	public:
		// bsf : best so far of each individual
		// hb: all historical best solutions of each individual
		// ci: improved individuals in the best so far population
		// c: all individuals in the current population
		enum UpdateScheme { bsf, hb, ci, c };
	protected:
		UpdateScheme m_ms = UpdateScheme::bsf;			//memory scheme

		std::vector<double> m_fitness;
		std::vector<double> m_weight;
		std::vector<double> m_obj;

		double m_memoryMaxObj, m_memoryMinObj, m_preMemoryMaxObj, m_preMemoryMinObj;
		double m_wt = 1.e-3;			// weight threshold, individuals with weight less than m_wt are removed from memory

		std::vector<std::deque<int>> m_exMemory; // explicit memory: the index of all persnal best memory 

		std::vector<typename Population::individual_type> m_hisIndi;		//historical individuals
		std::vector<typename Population::individual_type> m_curPop;
		std::unique_ptr<adaptor<typename Population::individual_type> > m_adaptor;
	public:
		GL(param_map &v);
		GL(int size, double alpha = -1, UpdateScheme scheme = UpdateScheme::bsf);
		void initilizeMemory();
		bool ifTerminating();
		void updateMemory(bool excute = false);
		void updateMemory_HB(const std::vector<int>& index);
		void updateMemory_BSF(const std::vector<int>& index);
		void updateMemory_CI(const std::vector<int>& index);
		void updateMemory_C();
		virtual void initilizeCurPop();
		int getNumberImpr() {
			return m_impRadio;
		}
		void resetFlag() {
			for (int i = 0; i < this->m_popsize; ++i)
				this->m_pop[i]->setFlag(false);
		}
	protected:
		evaluation_tag updata();
		int m_impRadio;
		double m_alpha, m_beta = 0, m_gamma = 6;

	};


	template<typename Population>
	GL<Population>::GL(param_map &v) :TypePop(int(v[param_popSize])), m_fitness(v[param_popSize])\
		, m_weight(v[param_popSize]), m_obj(v[param_popSize]), m_exMemory(v[param_popSize]), m_impRadio(0){

		if (v.find(param_updateSchemeProbabilityLearning) != v.end())	m_ms = (UpdateScheme)(int)v[param_updateSchemeProbabilityLearning];

		if (v.find(param_xoverProbability) != v.end())	m_alpha = v[param_xoverProbability];
		else m_alpha = 0.9;

		if (v.find(param_beta) != v.end()) m_beta = v[param_beta];

		if (v.find(param_gamma) != v.end()) m_gamma = v[param_gamma];
		this->m_term.reset(new TermMean(v));

	}

	template<typename Population>
	GL<Population>::GL(int size, double alpha, UpdateScheme scheme) :TypePop(size), m_fitness(size)\
		, m_weight(size), m_obj(size), m_exMemory(size), m_impRadio(0), m_alpha(alpha), m_ms(scheme){

	}

	template<typename Population>
	void GL<Population>::initilizeCurPop()
	{
		m_curPop.resize(this->m_popsize);
		for (int i = 0; i < this->m_popsize; i++) {
			m_curPop[i].initialize(this->m_pop[i]->data().m_x[0]);
		}
	}

	template<typename Population>
	void GL<Population>::initilizeMemory() {
		m_memoryMaxObj = m_memoryMinObj = (*this->m_pop[0]).data().m_obj[0];
		for (int i = 0; i < this->m_popsize; ++i) {
			double obj = this->m_pop[i]->data().m_obj[0];
			if (obj > m_memoryMaxObj) m_memoryMaxObj = obj;
			if (obj < m_memoryMinObj) m_memoryMinObj = obj;
		}
		//std::vector<int> indiv(this->m_numDim);
		double gap = m_memoryMaxObj - m_memoryMinObj + 1;
		for (int i = 0; i < this->m_popsize; ++i) {
			m_obj[i] = this->m_pop[i]->data().m_obj[0];
			if (Global::msp_global->mp_problem->getOptType() == MIN_OPT)	m_fitness[i] = (m_memoryMaxObj - m_obj[i] + 1) / gap;
			else m_fitness[i] = (m_obj[i] - m_memoryMinObj + 1) / gap;

			m_weight[i] = 1. / (1 + exp(-m_fitness[i]));
			m_exMemory[i].push_front(i);
			//for (int j = 0; j < this->m_numDim; j++)
			//	indiv[j] = this->m_pop[i]->data().m_x[j];
			//m_hisIndi.push_back(indiv);
			m_hisIndi.push_back(*this->m_pop[i]);
		}

		m_adaptor->updatePro(this->m_pop, m_weight);

		m_preMemoryMaxObj = m_memoryMaxObj;
		m_preMemoryMinObj = m_memoryMinObj;
	}

	template<typename Population>
	void GL<Population>::updateMemory(bool excute) {

		std::vector<int> index;  //the updated individual in the best so far

		if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			if (!excute) {
				m_memoryMinObj = m_preMemoryMinObj;
				for (int i = 0; i < this->m_popsize; ++i)
				{
					if (this->m_pop[i]->getFlag() == true)
					{
						index.push_back(i);
						if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
							m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
					}
				}
			}
			else {
				m_memoryMinObj = numeric_limits<double>::max();
				for (int i = 0; i < this->m_popsize; ++i) {
					index.push_back(i);
					if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
						m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
				}
			}

		}
		else {
			if (!excute) {
				m_memoryMaxObj = m_preMemoryMaxObj;
				for (int i = 0; i < this->m_popsize; ++i)
				{
					if (this->m_pop[i]->getFlag() == true)
					{
						index.push_back(i);
						if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
							m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
					}
				}
			}
			else {
				m_memoryMaxObj = numeric_limits<double>::min();
				for (int i = 0; i < this->m_popsize; ++i)
				{
					index.push_back(i);
					if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
						m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
				}
			}
		}

		if (m_ms != GL::c)
			if (index.empty())
				return;

		switch (m_ms)
		{
		case GL::bsf:
			updateMemory_BSF(index);
			break;
		case GL::hb:
			updateMemory_HB(index);
			break;
		case GL::ci:
			updateMemory_CI(index);
			break;
		case GL::c:
			updateMemory_C();
			break;
		default:
			break;
		}

	}

	template<typename Population>
	void GL<Population>::updateMemory_HB(const std::vector<int>& index)
	{
		if (m_memoryMinObj < m_preMemoryMinObj)
			m_preMemoryMinObj = m_memoryMinObj;

		double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			if (m_memoryMinObj < m_preMemoryMinObj)
			{
				for (int i = 0; i < m_weight.size(); ++i)
				{
					m_fitness[i] = (m_preMemoryMaxObj - m_obj[i] + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
		}
		else {
			if (m_memoryMinObj < m_preMemoryMinObj)
			{
				for (int i = 0; i < m_weight.size(); ++i)
				{
					m_fitness[i] = (m_preMemoryMaxObj - m_obj[i] + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
		}




		std::vector<double> weight(index.size()), fitness(index.size());
		for (int i = 0; i < weight.size(); ++i)
		{
			fitness[i] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0]) / gap;
			weight[i] = 1 / (1 + exp(-fitness[i]));
		}
		int z = 0; //改进个体按顺序1，2，...出现
		for (int i = 0; i < m_exMemory.size(); ++i)
		{
			int exMemorySize = m_exMemory[i].size();
			for (int j = 0; j < exMemorySize; ++j)
			{
				if (this->m_pop[i]->getFlag() == true)
				{
					if (m_memoryMinObj < m_preMemoryMinObj)
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / (pow((j + 2), 2));
					else
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] * pow((j + 1), 2) / pow((j + 2), 2);
					if (j == m_exMemory[i].size() - 1)
					{
						if (m_weight[m_exMemory[i][j]] / weight[z] < m_wt) {
							int pos = m_exMemory[i][j];
							m_weight[pos] = weight[z];
							m_obj[pos] = this->m_pop[i]->data().m_obj[0];
							m_fitness[pos] = fitness[z];
							m_exMemory[i].pop_back();
							m_exMemory[i].push_front(pos);
							for (int e1 = 0; e1 < this->m_numDim; e1++)
								m_hisIndi[pos].data().m_x[e1] = this->m_pop[i]->data().m_x[e1];
						}
						else {
							m_obj.push_back(this->m_pop[i]->data().m_obj[0]);
							m_fitness.push_back(fitness[z]);
							m_weight.push_back(weight[z]);
							m_exMemory[i].push_front(m_weight.size() - 1);
							m_hisIndi.push_back(*this->m_pop[i]);
						}
						z++;
					}
				}
				else
				{
					if (m_memoryMinObj < m_preMemoryMinObj)
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / pow((j + 1), 2);
				}
			}
		}

		m_adaptor->updatePro(m_hisIndi, m_weight);

		std::vector<double>::iterator maxObj = max_element(m_obj.begin(), m_obj.end());
		m_preMemoryMaxObj = *maxObj;
	}

	template<typename Population>
	void GL<Population>::updateMemory_BSF(const std::vector<int>& index)
	{
		/*if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			m_memoryMaxObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
					m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < this->m_popsize; i++) {
					m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->data().m_obj[0] + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < index.size(); ++i)
				{
					m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0] + 1) / gap;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		else {
			m_memoryMinObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
					m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj )
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < this->m_popsize; i++) {
					m_fitness[i] = (this->m_pop[i]->data().m_obj[0] - m_preMemoryMinObj + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < index.size(); ++i)
				{
					m_fitness[index[i]] = (this->m_pop[index[i]]->data().m_obj[0] - m_preMemoryMinObj + 1) / gap;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		*/
		/*if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			m_memoryMaxObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
					m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < this->m_popsize; i++) {
					if(gap>0)				m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->data().m_obj[0]) / gap;
					else m_fitness[i] = 0;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < index.size(); ++i)
				{
					if(gap>0)	m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0]) / gap;
					else m_fitness[index[i]] = 0;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		else {
			m_memoryMinObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
					m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj)
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < this->m_popsize; i++) {
					if(gap>0)
					m_fitness[i] = (this->m_pop[i]->data().m_obj[0] - m_preMemoryMinObj) / gap;
					else m_fitness[i] = 0;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < index.size(); ++i)
				{
					if(gap>0)				m_fitness[index[i]] = (this->m_pop[index[i]]->data().m_obj[0] - m_preMemoryMinObj) / gap;
					else m_fitness[index[i]] = 0;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}*/
		if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			m_memoryMaxObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
					m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < this->m_popsize; i++) {
						m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->data().m_obj[0]) / gap;
						m_weight[i] = pow(m_fitness[i], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[i]));
					}
				}
				else {
					for (int i = 0; i < this->m_popsize; i++) {
						m_fitness[i] = 0;
						m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
					}
				}

			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0]) / gap;
						m_weight[index[i]] = pow(m_fitness[index[i]], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[index[i]]));
					}
				}
				else {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = 0;
						m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
					}
				}

			}
		}
		else {
			m_memoryMinObj = this->m_pop[0]->data().m_obj[0];
			for (int i = 1; i < this->m_popsize; ++i) {
				if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
					m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj)
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < this->m_popsize; i++) {
						m_fitness[i] = (this->m_pop[i]->data().m_obj[0] - m_preMemoryMinObj) / gap;
						m_weight[i] = pow(m_fitness[i], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[i]));
					}
				}
				else {
					for (int i = 0; i < this->m_popsize; i++) {
						m_fitness[i] = 0;
						m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
					}
				}

			}
			else {
				double gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = (this->m_pop[index[i]]->data().m_obj[0] - m_preMemoryMinObj) / gap;
						m_weight[index[i]] = pow(m_fitness[index[i]], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[index[i]]));
					}
				}
				else {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = 0;
						m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
					}
				}

			}
		}
		m_adaptor->updatePro(this->m_pop, m_weight);
	}

	template<typename Population>
	void GL<Population>::updateMemory_CI(const std::vector<int>& index)
	{
		m_preMemoryMaxObj = this->m_pop[index[0]]->data().m_obj[0];
		m_preMemoryMinObj = this->m_pop[index[0]]->data().m_obj[0];
		for (int i = 1; i < index.size(); ++i)
		{
			if (m_preMemoryMaxObj < this->m_pop[index[i]]->data().m_obj[0])
				m_preMemoryMaxObj = this->m_pop[index[i]]->data().m_obj[0];
			if (m_preMemoryMinObj > this->m_pop[index[i]]->data().m_obj[0])
				m_preMemoryMinObj = this->m_pop[index[i]]->data().m_obj[0];
		}
		double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < index.size(); ++i)
		{
			if (Global::msp_global->mp_problem->getOptType() == MIN_OPT)	m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0] + 1) / gap;
			else m_fitness[index[i]] = (this->m_pop[index[i]]->data().m_obj[0] - m_preMemoryMinObj + 1) / gap;

			m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
		}

		m_adaptor->updatePro(this->m_pop, m_weight, &index);
	}

	template<typename Population>
	void GL<Population>::updateMemory_C()
	{
		m_preMemoryMaxObj = m_curPop[0].data().m_obj[0];
		m_preMemoryMinObj = m_curPop[0].data().m_obj[0];
		for (int i = 1; i < this->m_popsize; ++i)
		{
			if (m_preMemoryMaxObj < m_curPop[i].data().m_obj[0])
				m_preMemoryMaxObj = m_curPop[i].data().m_obj[0];
			if (m_preMemoryMinObj > m_curPop[i].data().m_obj[0])
				m_preMemoryMinObj = m_curPop[i].data().m_obj[0];
		}
		double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < this->m_popsize; ++i)
		{
			if (Global::msp_global->mp_problem->getOptType() == MIN_OPT)	m_fitness[i] = (m_preMemoryMaxObj - m_curPop[i].data().m_obj[0] + 1) / gap;
			else m_fitness[i] = (m_curPop[i].data().m_obj[0] - m_preMemoryMinObj + 1) / gap;
			m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
		}

		m_adaptor->updatePro(m_curPop, m_weight);
	}

	template<typename Population>
	bool GL<Population>::ifTerminating()
	{
		//TermMean * term = dynamic_cast<TermMean*>(this->m_term.get());
		if (this->m_term->ifTerminating()) {
			if (this->m_iter == 0) return false;
			return true;
		}

		return false;
	}

	template<typename Population>
	evaluation_tag GL<Population>::updata()
	{
		evaluation_tag rf = Normal;

		rf = m_adaptor->updateSolutions(this->m_pop, m_curPop, m_impRadio, m_ms);

		return rf;
	}

}

#endif // !GENETIC_LEARNING_H

