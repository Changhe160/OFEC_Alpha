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

#ifndef DTLZ4_H
#define DTLZ4_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ4 final : public DTLZ
	{
	public:
		DTLZ4(param_map &v);
		DTLZ4(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ4() {}; 
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ4_H  