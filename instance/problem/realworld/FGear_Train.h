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

/**************************************************************************
design of a gear train,  which
was introduced in
    E.~Sandgren, ``Nonlinear integer and discrete programming in mechanical
    design,'' in \emph{the ASME Design Technology Conf.,}, 1988, pp. 95--105.
, is to optimize the gear ratio for a compound gear train that contains three
gears. It is to be designed that the gear ratio is as close as possible to
1/6.931. For each gear, the number of teeth must be between
12 and 60.
******************************************************************************/


#ifndef OFEC_FGEAR_TRAIN_H
#define OFEC_FGEAR_TRAIN_H

#include "../../../core/problem/continuous/function.h"
namespace OFEC {
	class FGear_Train : public function
	{
	public:
		FGear_Train(param_map &v);
		FGear_Train(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
}
#endif // OFEC_FGEAR_TRAIN_H
