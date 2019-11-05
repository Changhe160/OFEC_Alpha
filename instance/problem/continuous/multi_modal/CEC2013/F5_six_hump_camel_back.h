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
*  Paper: Benchmark Functions for CEC��2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/

#ifndef OFEC_F5_SIX_HUMP_CAMEL_BACK_H
#define OFEC_F5_SIX_HUMP_CAMEL_BACK_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2013 {
		// An inverted version of Six-hump Camel Back function
		class F5_six_hump_camel_back final : public function
		{
		public:
			F5_six_hump_camel_back(param_map &v);
			F5_six_hump_camel_back(const std::string &name);
			void initialize();
		protected:
			void evaluate_objective(real *x, std::vector<real>& obj) override;
		};
	}
}
#endif // !OFEC_F5_SIX_HUMP_CAMEL_BACK_H

