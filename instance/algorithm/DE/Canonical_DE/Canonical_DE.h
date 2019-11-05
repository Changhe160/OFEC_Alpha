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
*********************************************************************************/
// Created on 15th Aug, 2019 by Junchen Wang

#ifndef OFEC_CANONICAL_DE_H
#define OFEC_CANONICAL_DE_H

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class Canonical_DE : public algorithm {
	public:
		Canonical_DE(param_map& v);
		void initialize() override;
		void run_() override;
		void record() override;
	protected:
		DE::population<DE::individual> m_pop;
	};

	class DE_rand_1 final : public Canonical_DE {
	public:
		DE_rand_1(param_map& v);
		void initialize() override;
	};

	class DE_best_2 final : public Canonical_DE {
	public:
		DE_best_2(param_map& v);
		void initialize() override;
	};

	class DE_nrand_1 final : public Canonical_DE {
	public:
		DE_nrand_1(param_map& v);
		void initialize() override;
	};
}

#endif // !OFEC_CANONICAL_DE_H

