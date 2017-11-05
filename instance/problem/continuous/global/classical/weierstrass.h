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

#ifndef OFEC_WEIERSTRASS_H
#define OFEC_WEIERSTRASS_H
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class weierstrass : public function
	{
	public:
		weierstrass(param_map &v);
		weierstrass(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
		double m_a = 0.5, m_b = 3;
		int m_kmax = 20;
	};
	
}
#endif // !OFEC_WEIERSTRASS_H
