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


#ifndef OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H
#define OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H

#include "../elliptic.h"

namespace OFEC {
	class F3_shifted_rotated_high_cond_elliptic : public elliptic
	{
	public:
		F3_shifted_rotated_high_cond_elliptic(param_map &v);
		F3_shifted_rotated_high_cond_elliptic(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F3_shifted_rotated_high_cond_elliptic();
	protected:
		void initialize();
		void evaluate__(real *x, vector<real>& obj);
	private:
	};
}
#endif // ! OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H
