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

#ifndef OFEC_F9_GLOBAL_COMPOSITION1_H
#define OFEC_F9_GLOBAL_COMPOSITION1_H

#include "../../expensive/CEC2015/composition_2015.h"

namespace OFEC {
	namespace CEC2015 {
		class F9_global_composition1 final : public composition_2015
		{
		public:
			F9_global_composition1(param_map &v);
			F9_global_composition1(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();

		private:

		};
	}
	using CEC2015_GOP_F9 = CEC2015::F9_global_composition1;
}
#endif // !OFEC_F9_GLOBAL_COMPOSITION1_H

