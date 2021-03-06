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

#ifndef OFEC_MODIFIED_SCHWEFEL_H
#define OFEC_MODIFIED_SCHWEFEL_H

#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {

	class modified_schwefel : public function
	{
	public:
		modified_schwefel(param_map &v);
		modified_schwefel(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluate_objective(real *x, std::vector<real>& obj) override;
	private:
	};

}
#endif // !OFEC_MODIFIED_SCHWEFEL_H
