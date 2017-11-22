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

#ifndef SHIFTEDSCHWEFEL_F15_H
#define SHIFTEDSCHWEFEL_F15_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class ShiftedSchwefel_F15 final:public function_CEC2013 {
		public:
			ShiftedSchwefel_F15(param_map &v);
			ShiftedSchwefel_F15(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ShiftedSchwefel_F15();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F15 = CEC2013::ShiftedSchwefel_F15;
}
#endif




