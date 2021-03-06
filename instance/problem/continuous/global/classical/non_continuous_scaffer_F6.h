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

#ifndef OFEC_NON_CONTINUOUS_SCAFFER_F6_H
#define OFEC_NON_CONTINUOUS_SCAFFER_F6_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	

	class non_continuous_scaffer_F6 : public function
	{
	public:
		non_continuous_scaffer_F6(param_map &v);
		non_continuous_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluate_objective(real *x, std::vector<real>& obj) override;
	private:
	};
	
}
#endif // ! OFEC_NON_CONTINUOUS_SCAFFER_F6_H


