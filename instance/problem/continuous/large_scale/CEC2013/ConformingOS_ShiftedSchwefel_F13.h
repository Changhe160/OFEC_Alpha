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

#ifndef CONFORMINGOS_SHIFTEDSCHWEFEL_F13_H
#define CONFORMINGOS_SHIFTEDSCHWEFEL_F13_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class ConformingOS_ShiftedSchwefel_F13 final:public function_CEC2013 {
		public:
			ConformingOS_ShiftedSchwefel_F13(param_map &v);
			ConformingOS_ShiftedSchwefel_F13(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ConformingOS_ShiftedSchwefel_F13();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F13 = CEC2013::ConformingOS_ShiftedSchwefel_F13;
}
#endif





