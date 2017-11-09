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


#ifndef OFEC_EXPANDED_FIVE_UNEVEN_PEAK_TRAP_H
#define OFEC_EXPANDED_FIVE_UNEVEN_PEAK_TRAP_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class expanded_five_uneven_peak_trap : public function
	{
	public:
		expanded_five_uneven_peak_trap(param_map &v);
		expanded_five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};

}
#endif // !OFEC_EXPANDED_FIVE_UNEVEN_PEAK_TRAP_H

