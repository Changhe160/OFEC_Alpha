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

#ifndef OFEC_NONCONT_RASTRIGIN_H
#define OFEC_NONCONT_RASTRIGIN_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class noncont_rastrigin : public function
	{
	public:
		noncont_rastrigin(param_map &v);
		noncont_rastrigin(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~noncont_rastrigin();
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
}
#endif // !OFEC_NONCONT_RASTRIGIN_H
