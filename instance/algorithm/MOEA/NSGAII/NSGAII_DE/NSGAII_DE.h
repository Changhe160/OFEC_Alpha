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

*************************************************************************/
// Created: 13 Jan 2015
// Last modified: 3 May 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_NSGAII_DE_H
#define OFEC_NSGAII_DE_H

#include "../NSGAII.h"
#include "../../../DE/population.h"
#include "../../../DE/MOEA_DE/MOEA_DE.h"

namespace OFEC {	
	class NSGAII_DE_pop : public DE::MOEA_DE_pop<>, NSGAII<DE::individual> {
	public:
		explicit NSGAII_DE_pop(size_t size_pop);
		void initialize() override;
		evaluation_tag evolve() override;
	protected:
		std::vector<DE::individual> m_offspring;  // 2 size of population
	};

	class NSGAII_DE : public algorithm {
	public:
		explicit NSGAII_DE(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		NSGAII_DE_pop m_pop;
	};
}

#endif // !OFEC_NSGAII_DE_H
