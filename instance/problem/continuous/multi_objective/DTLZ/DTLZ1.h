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
// Created: 12 JAN 2014
// Modified: 29 Mar 2018 by Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef DTLZ1_H
#define DTLZ1_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ1 final : public DTLZ
	{
	public:
		DTLZ1(param_map &v);
		DTLZ1(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ1() {};
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif //DTLZ1_H  