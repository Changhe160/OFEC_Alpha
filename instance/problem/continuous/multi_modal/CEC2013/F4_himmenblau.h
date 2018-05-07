/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen.chris@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************
*  Paper: Benchmark Functions for CEC¡¯2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/

#ifndef OFEC_F4_HIMMENBLAU_H
#define OFEC_F4_HIMMENBLAU_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2013 {
		// An inverted version of Himmelblau function
		class F4_himmenblau final : public function
		{
		public:
			F4_himmenblau(param_map &v);
			F4_himmenblau(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:
			void evaluate__(real *x, std::vector<real>& obj);
		};
	}
}
#endif // !OFEC_F4_HIMMENBLAU_H

