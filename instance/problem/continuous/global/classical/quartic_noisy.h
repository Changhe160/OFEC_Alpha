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

#ifndef OFEC_QUARTIC_NOISY_H
#define OFEC_QUARTIC_NOISY_H
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class quartic_noisy : public function
	{
	public:
		quartic_noisy(param_map &v);
		quartic_noisy(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}
#endif // !OFEC_QUARTIC_NOISY_H
