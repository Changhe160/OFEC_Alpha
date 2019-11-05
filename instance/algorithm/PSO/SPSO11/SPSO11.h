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
SPSO11: C. Maurice, "Standard PSO 2011 (SPSO-2011)" http://www.particleswarm.info/Programs.html, 2011.
 M. Clerc, Standard Particle Swarm Optimisation, Particle Swarm Central, Tech.Rep., 2012,
*/
#ifndef SPSO11_H
#define SPSO11_H

#include "../../../../core/algorithm/algorithm.h"
#include "../particle.h"
#include "../swarm.h"

namespace OFEC {


	class particle11 final : public particle {
	public:
		particle11() = default;
		particle11(size_t num_obj, size_t num_con, size_t size_var) : particle(num_obj, num_con, size_var) {}
		//particle11(const solution<> &rhs) :particle(rhs) {}
		void initialize_velocity() override;
		void clamp_velocity() override;
		void next_velocity(const solution<> &lbest, real w, real c1, real c2) override;
	};

	class SPSO11 final : public algorithm {
	public:
		SPSO11(param_map& v);
		void initialize() override;
		void run_() override;
		void record() override;
	protected:
		swarm<particle11> m_pop;
	};
}
#endif // SPSO11_H
