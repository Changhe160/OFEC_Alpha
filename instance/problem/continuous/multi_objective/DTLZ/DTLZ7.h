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
// Created: 7 Oct 2018 by Junchen Wang (wangjunchen.chris@gmail.com)
// Modified: 

#ifndef OFEC_DTLZ7_H
#define OFEC_DTLZ7_H

#include "DTLZ.h"

//Note: It was named after DTLZ7 in [Deb2001] and source file "dtlz_c_source.tar.gz"

namespace OFEC {
	class DTLZ7 final : public DTLZ {
	public:
		DTLZ7(param_map &v);
		DTLZ7(const std::string &name, size_t size_var, size_t size_obj);
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif // !OFEC_DTLZ7_H
