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

#ifndef OFEC_MODIFIED_SHEKEL_H
#define OFEC_MODIFIED_SHEKEL_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	

	class modified_shekel : public function
	{
	public:
		modified_shekel(param_map &v);
		modified_shekel(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
		double m_a[8][5];
		double m_c[8];
	};
	
}
#endif // !OFEC_MODIFIED_SHEKEL_H