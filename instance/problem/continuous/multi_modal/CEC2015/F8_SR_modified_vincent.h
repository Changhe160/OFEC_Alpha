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
******************************************************************************************
*  Paper: Problem Definitions and Evaluation Criteria for the CEC 2015
*  Competition on Single Objective Multi-Niche Optimization.
*******************************************************************************************/


#ifndef OFEC_F8_SR_MODIFIED_VINCENT_H
#define OFEC_F8_SR_MODIFIED_VINCENT_H

#include "CEC2015_function.h"

namespace OFEC {
	namespace CEC2015 {
		class F8_SR_modified_vincent final : public CEC2015_function
		{
		public:
			F8_SR_modified_vincent(param_map &v);
			F8_SR_modified_vincent(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2015_MMP_F8 = CEC2015::F8_SR_modified_vincent;
}
#endif // !OFEC_F8_SR_MODIFIED_VINCENT_H




