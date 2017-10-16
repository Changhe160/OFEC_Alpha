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


#ifndef OFEC_F9_SHIFTED_RASTRIGIN_H
#define OFEC_F9_SHIFTED_RASTRIGIN_H

#include "../rastrigin.h"

namespace OFEC {
	class F9_shifted_rastrigin : public rastrigin
	{
	public:
		F9_shifted_rastrigin(param_map &v);
		F9_shifted_rastrigin(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F9_shifted_rastrigin();
	protected:
		void initialize();
		void evaluate__(real *x, vector<real>& obj);
	private:
	};
}
#endif // ! OFEC_F9_SHIFTED_RASTRIGIN_H


