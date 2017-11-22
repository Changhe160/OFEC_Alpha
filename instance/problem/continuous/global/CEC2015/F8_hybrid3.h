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
#ifndef OFEC_F8_HYBRID3_H
#define OFEC_F8_HYBRID3_H

#include "../../expensive/CEC2015/hybrid.h"
namespace OFEC {
	namespace CEC2015 {
		class F8_hybrid3 final : public hybrid
		{
		public:
			F8_hybrid3(param_map &v);
			F8_hybrid3(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
		private:
		};
	}
	using CEC2015_GOP_F8 = CEC2015::F8_hybrid3;
}

#endif // !OFEC_F8_HYBRID3_H








