/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia & Junchen Wang
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 15 Aug 2019 by Junchen Wang (email:wangjunchen.chris@gmail.com)

#ifndef OFEC_NSGAIII_SBXRM_H
#define OFEC_NSGAIII_SBXRM_H

#include "../NSGAIII.h"
#include "../../../../../core/algorithm/individual.h"
#include "../../../GA/SBX/SBX.h"

namespace OFEC {
	class NSGAIII_SBX_pop : public SBX_pop<>, NSGAIII<individual<>> {
	public:
		explicit NSGAIII_SBX_pop(size_t size_pop);
		void initialize() override;
		evaluation_tag evolve() override;
	protected:
		std::vector<individual<>> m_offspring;  // 2 size of population
	};

	class NSGAIII_SBX : public algorithm {
	public:
		explicit NSGAIII_SBX(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
		NSGAIII_SBX_pop m_pop;
	};
}

#endif // !OFEC_NSGAIII_SBXRM_H

