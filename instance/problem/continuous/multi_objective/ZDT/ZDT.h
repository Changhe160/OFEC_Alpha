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

/*
Deb, K., Thiele, L., Laumanns, M., & Zitzler, E. (2002, May).
Scalable multi-objective optimization test problems.
In Evolutionary Computation, 2002. CEC'02. Proceedings of the 2002 Congress on (Vol. 1, pp. 825-830). IEEE.
*/

#ifndef ZDT_H
#define ZDT_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class ZDT :public continuous {
	public:
		void initialize();
	protected:
		ZDT(const std::string &name, size_t size_var);
		void generateAdLoadPF();
	};
}



#endif //ZDT_H