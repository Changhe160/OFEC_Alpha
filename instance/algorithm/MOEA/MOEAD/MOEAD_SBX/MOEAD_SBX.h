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
// Last modified: 25 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)


#ifndef OFEC_MOEAD_SBX_H
#define OFEC_MOEAD_SBX_H

#include "../MOEAD.h"
#include "../../../GA/SBX/SBX.h"


namespace OFEC {
	class MOEAD_SBX_pop :public SBX_pop<>, MOEAD<individual<>> {
	public:
		MOEAD_SBX_pop(size_t size_pop);
		void initialize();
		evaluation_tag evolve();
	protected:
		evaluation_tag evolve_mo();
	};

	class MOEAD_SBX :public algorithm {
	protected:
		MOEAD_SBX_pop m_pop;
	public:
		MOEAD_SBX(param_map &v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	};

}

#endif
