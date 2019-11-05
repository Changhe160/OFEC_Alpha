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
// Last modified: 24 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)


#ifndef OFEC_MOEAD_DE_H
#define OFEC_MOEAD_DE_H

#include "../MOEAD.h"
#include "../../../DE/MOEA_DE/MOEA_DE.h"

namespace OFEC {
	class MOEAD_DE_pop :public DE::MOEA_DE_pop<>, MOEAD<DE::individual> {
	public:
		MOEAD_DE_pop(size_t size_pop);
		void initialize();
		evaluation_tag evolve();
	protected:
		evaluation_tag evolve_mo();
	};

	class MOEAD_DE :public algorithm {
	protected:
		MOEAD_DE_pop m_pop;
	public:
		MOEAD_DE(param_map &v);
		void initialize() override; 
		void record() override;
	protected:
		void run_() override;
	};

}

#endif
