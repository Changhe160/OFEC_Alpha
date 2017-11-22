/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_SHIFTEDELLIPTIC_F1_H
#define OFEC_SHIFTEDELLIPTIC_F1_H

#include "function_CEC2013.h"

namespace OFEC {
	namespace CEC2013 {
		class ShiftedElliptic_F1 final :public function_CEC2013 {
		public:
			ShiftedElliptic_F1(param_map &v);
			ShiftedElliptic_F1(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ShiftedElliptic_F1();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F1 = CEC2013::ShiftedElliptic_F1;
}
#endif // ! OFEC_SHIFTEDELLIPTIC_F1_H