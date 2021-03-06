/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#ifndef OFEC_VINCENT_H
#define OFEC_VINCENT_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class vincent : public function
	{
	public:
		vincent(param_map &v);
		vincent(const std::string &name, size_t size_var);
		void initialize();

	protected:
		void evaluate_objective(real *x, std::vector<real>& obj) override;
	private:
	};
	
}
#endif //!OFEC_VINCENT_H