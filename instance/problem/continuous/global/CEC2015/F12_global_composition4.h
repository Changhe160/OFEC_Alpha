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
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015
*  Competition on Learning-based Real-Parameter Single Objective
*  Optimization.
************************************************************************/

#ifndef OFEC_F12_GLOBAL_COMPOSITION4_H
#define OFEC_F12_GLOBAL_COMPOSITION4_H

#include "../../expensive/CEC2015/composition_2015.h"

namespace OFEC {
	namespace CEC2015 {
		class F12_global_composition4 final : public composition_2015
		{
		public:
			F12_global_composition4(param_map &v);
			F12_global_composition4(const std::string &name, size_t size_var, size_t size_obj);

		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();

		protected:

		};
	}
	using CEC2015_GOP_F12 = CEC2015::F12_global_composition4;
}
#endif // !OFEC_F12_GLOBAL_COMPOSITION4_H



