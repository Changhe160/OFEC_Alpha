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

#ifndef OFEC_ROTATED_SCAFFER_F6_H
#define OFEC_ROTATED_SCAFFER_F6_H


#include "scaffer_F6.h"

namespace OFEC {
	
	class rotated_scaffer_F6 : public scaffer_F6
	{
	public:
		rotated_scaffer_F6(param_map &v);
		rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}
#endif // ! OFEC_ROTATED_SCAFFER_F6_H

