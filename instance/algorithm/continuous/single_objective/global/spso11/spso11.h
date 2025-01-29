/********* Begin Register Information **********
{
	"name": "SPSO-2011",
	"identifier": "SPSO11",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 21 September 2011
// Last modified: 21 Nov 2014

/*
"Standard PSO 2011 (SPSO-2011)" http://www.particleswarm.info/Programs.html
@inproceedings{zambrano2013standard,
  pages = {2337--2344},
  year = {2013},
  author = {Mauricio Zambrano-Bigiarini and Maurice Clerc and Rodrigo Rojas},
  organization = {IEEE},
  title = {Standard particle swarm optimisation 2011 at {CEC}-2013: A baseline for future {PSO} improvements},
  booktitle = {2013 IEEE Congress on Evolutionary Computation}
}
*/

#ifndef OFEC_SPSO11_H
#define OFEC_SPSO11_H

#include "../../../../../../core/algorithm/algorithm.h"
#include "particle11.h"
#include "../../../../template/classic/particle_swarm_optimization/swarm.h"

namespace ofec {
	class SPSO11 : virtual public Algorithm {
		OFEC_CONCRETE_INSTANCE(SPSO11)
	protected:
		std::unique_ptr<Swarm<Particle11>> m_pop;
		Real m_weight, m_accelerator1, m_accelerator2;
		size_t m_pop_size;

		void addInputParameters();
		void initialize_(Environment *env) override;
		void run_(Environment *env) override;
	};
}
#endif // OFEC_SPSO11_H
