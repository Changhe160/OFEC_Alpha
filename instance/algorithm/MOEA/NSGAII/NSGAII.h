/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*  See the details of NSGA2 in the following paper
*  A Fast and Elitist Multiobjective Genetic Algorithm: NSGA-II
*  Kalyanmoy Deb, Associate Member, IEEE, Amrit Pratap, Sameer Agarwal, and T. Meyarivan
*  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL. 6, NO. 2, APRIL 2002
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 29 Mar 2018 by Junchen Wang

#ifndef NSGAII_H
#define NSGAII_H

#include "../../../../core/algorithm/population.h"
#include "../../../../core/algorithm/individual.h"
#include <list>
#include "../../../../core/measure/measure.h"

namespace OFEC {
	class NSGAII : public population<individual<>> {
	public:
		NSGAII(param_map &v);
		NSGAII(const std::string& name, int size_pop, int max_evals);
		~NSGAII() {}
		evaluation_tag run_();
		void sort(); 
	protected:
		void eval_dens(); 
		virtual void evolve_mo();
		int tour_selection();
    protected:
		std::vector<std::shared_ptr<individual<>>> m_offspring;
		double m_cr;
		double m_ceta;
		double m_mr;
		double m_meta;
	private:
		void cross_mutate(const std::vector<int> &index, individual<> *child1, individual<> *child2);
		double get_betaq(double rand, double alpha, double ceta);
		void PolynomialMutation(individual<> *indv, double mr, double meta);
		void SimulatedBinaryCrossover(individual<> *child1, individual<> *child2, const individual<> &parent1, const individual<> &parent2, double cr, double ceta);
		void setCrossXP(double cr) { m_cr = cr; }
		void setMutationP(double mr) { m_mr = mr; }
		void setEta(double ceta, double meta) { m_ceta = ceta; m_meta = meta; }
	};
}

#endif //!NSGAII_H
