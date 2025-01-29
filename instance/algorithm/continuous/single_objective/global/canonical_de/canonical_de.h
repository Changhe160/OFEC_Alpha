/********* Begin Register Information **********
{
	"name": "Canonical-DE",
	"identifier": "CanonicalDE",
	"tags": [ "single-objective", "continuous" ]
}
*********** End Register Information **********/

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
*-------------------------------------------------------------------------------
* Created on 15th Aug, 2019 by Junchen Wang
*********************************************************************************/

/********************************************************************
@article{storn1997differential,
  volume = {11},
  number = {4},
  journal = {Journal of Global Optimization},
  pages = {341--359},
  year = {1997},
  author = {Rainer Storn and Kenneth Price},
  title = {Differential evolution--a simple and efficient heuristic for global optimization over continuous spaces}
}
********************************************************************/

#ifndef OFEC_CANONICAL_DE_H
#define OFEC_CANONICAL_DE_H

#include "../../../../template/classic/differential_evolution/population.h"
#include "../../../../../../core/algorithm/algorithm.h"

namespace ofec {
	class CanonicalDE : virtual public Algorithm {
		OFEC_CONCRETE_INSTANCE(CanonicalDE)
	protected:
		std::unique_ptr<PopulationDE<>> m_population;
		size_t m_population_size;
		Real m_scaling_factor, m_crossover_rate;
		de::MutateStrategy m_mutation_strategy;
		de::RecombineStrategy m_recombine_strategy;

		void addInputParameters();
		void initialize_(Environment *env) override;
		void run_(Environment *env) override;

		void initializePopulation(size_t pop_size, Environment *env);
	};
}

#endif // !OFEC_CANONICAL_DE_H

