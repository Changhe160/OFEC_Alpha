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


#ifndef OFEC_FSHUBERT_H
#define OFEC_FSHUBERT_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	class FShubert : public function
	{
	public:
		FShubert(param_map &v);
		FShubert(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~FShubert();
	protected:
		void initialize();
		void evaluate__(real *x, vector<real>& obj);
	private:
	};
}
#endif // ! OFEC_FSHUBERT_H