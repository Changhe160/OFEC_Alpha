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


#ifndef OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H
#define OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H

#include "F2_shifted_schwefel_1_2.h"

namespace OFEC {
	class F4_shifted_schwefel_1_2_noisy : public F2_shifted_schwefel_1_2
	{
	public:
		F4_shifted_schwefel_1_2_noisy(param_map &v);
		F4_shifted_schwefel_1_2_noisy(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F4_shifted_schwefel_1_2_noisy();
	protected:
		void initialize();
		void evaluate__(real *x, vector<real>& obj);
	private:
	};
}
#endif // ! OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H
