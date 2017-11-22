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
#ifndef OFEC_SHIFTEDRASTRIGIN_F2_H
#define OFEC_SHIFTEDRASTRIGIN_F2_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class ShiftedRastrigin_F2 final:public function_CEC2013 {
		public:
			ShiftedRastrigin_F2(param_map &v);
			ShiftedRastrigin_F2(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ShiftedRastrigin_F2();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F2 = CEC2013::ShiftedRastrigin_F2;
}
#endif // OFEC_SHIFTEDRASTRIGIN_F2_H
