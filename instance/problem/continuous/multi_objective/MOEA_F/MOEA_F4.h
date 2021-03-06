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

#ifndef MOEA_F4_H
#define MOEA_F4_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F4 final : public MOEA_FBase
	{
	public:
		MOEA_F4(param_map &v);
		MOEA_F4(const std::string &name, size_t size_var);
		~MOEA_F4() {}
		void initialize();
	};
}


#endif //MOEA_F4_H