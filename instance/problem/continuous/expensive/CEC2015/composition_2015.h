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

#ifndef OFEC_CEC2015_COMPOSITION_H
#define OFEC_CEC2015_COMPOSITION_H

#include "../../global/CEC2005/composition.h"
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2015 {
		class composition_2015 : public CEC2005::composition
		{
		public:
			composition_2015(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void evaluate__(real *x, std::vector<real>& obj);
		
			void set_weight(std::vector<double>& w, const std::vector<real>&x);

		protected:
			std::vector<real> m_f_bias;
		};

	}
}
#endif // ! OFEC_CEC2015_COMPOSITION_H
