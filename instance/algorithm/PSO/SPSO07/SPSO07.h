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
* SPSO07: C. Maurice, "Standard pso 2007 (spso-07)" http://www.particleswarm.info/Programs.html, 2007.
*
*********************************************************************************/
// Created: 21 September 2011
// Last modified: 21 Nov 2014

#ifndef SPSO07_H
#define SPSO07_H

#include "../../../../core/algorithm/algorithm.h"
#include "../particle.h"
#include "../swarm.h"

namespace OFEC {
	class particle07 final : public particle {
	public:
		particle07() = default;
		particle07(size_t num_obj, size_t num_con, size_t size_var) : particle(num_obj, num_con, size_var) {}
		//particle07(const solution<> &rhs) :particle(rhs) {}
		void initialize_velocity() override;
		void next_velocity(const solution<> &lbest, real w, real c1, real c2) override;
	};

	class SPSO07 final : public algorithm {
	public:
		SPSO07(param_map& v);
		void initialize() override;
		void run_() override;
		void record() override;
	protected:
		void update_parameters() override;
	protected:
		swarm<particle07> m_pop;
	};

}

#endif // SPSO07_H
