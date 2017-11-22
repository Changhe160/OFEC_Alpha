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

#ifndef CONFLICTINGOS_SHIFTEDSCHWEFEL_F14_H
#define CONFLICTINGOS_SHIFTEDSCHWEFEL_F14_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class ConflictingOS_ShiftedSchwefel_F14 :public function_CEC2013 {
		public:
			ConflictingOS_ShiftedSchwefel_F14(param_map &v);
			ConflictingOS_ShiftedSchwefel_F14(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ConflictingOS_ShiftedSchwefel_F14();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F14 = CEC2013::ConflictingOS_ShiftedSchwefel_F14;
}
#endif





