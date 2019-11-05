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
// Created: 31 December 2014
// Modified: 29 Mar 2018 by Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef ZDT2_H
#define ZDT2_H


#include "ZDT.h"

namespace OFEC {
	class ZDT2 final : public ZDT {
	public:
		ZDT2(param_map &v);
		ZDT2(const std::string &name, size_t size_var);
		~ZDT2() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif //ZDT2_H