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
*************************************************************************/

#ifndef OFEC_PENALIZED_2_H
#define OFEC_PENALIZED_2_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class penalized_2 : public function
	{
	public:
		penalized_2(param_map &v);
		penalized_2(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		double u(real x, double a, double k, double m)const;
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}
#endif // !OFEC_PENALIZED_2_H
