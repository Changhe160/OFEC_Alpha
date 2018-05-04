/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef N7S1_SHIFTEDSCHWEFEL_F7_H
#define N7S1_SHIFTEDSCHWEFEL_F7_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class N7S1_ShiftedSchwefel_F7 final:public function_CEC2013 {
		public:
			N7S1_ShiftedSchwefel_F7(param_map &v);
			N7S1_ShiftedSchwefel_F7(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~N7S1_ShiftedSchwefel_F7();

			void initialize();
		};
	}
	using CEC2013_LSOP_F7 = CEC2013::N7S1_ShiftedSchwefel_F7;
}
#endif

