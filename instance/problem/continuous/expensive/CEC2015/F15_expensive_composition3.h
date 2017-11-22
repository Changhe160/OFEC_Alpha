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

#ifndef OFEC_F15_EXPENSIVE_COMPOSITION3_H
#define OFEC_F15_EXPENSIVE_COMPOSITION3_H

#include "composition_2015.h"

namespace OFEC {
	namespace CEC2015 {
		class F15_expensive_composition3 : public composition_2015
		{
		public:
			F15_expensive_composition3(param_map &v);
			F15_expensive_composition3(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();

		private:

		};
	}
	using CEC2015_EOP_F15 = CEC2015::F15_expensive_composition3;
}
#endif // !OFEC_F15_EXPENSIVE_COMPOSITION3_H


