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

#ifndef OFEC_F15_COMPOSITION_F3_H
#define OFEC_F15_COMPOSITION_F3_H

#include "composition_2015.h"

namespace OFEC {
	namespace CEC2015 {
		class F15_composition_F3 : public composition_2015
		{
		public:
			F15_composition_F3(param_map &v);
			F15_composition_F3(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();

		private:

		};
	}
}
#endif // !OFEC_F15_COMPOSITION_F3_H


