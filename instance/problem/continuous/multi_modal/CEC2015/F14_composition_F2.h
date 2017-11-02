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

#ifndef OFEC_F14_COMPOSITION_F2_H
#define OFEC_F14_COMPOSITION_F2_H

#include "composite.h"

namespace OFEC {
	namespace CEC2015 {
		class F14_composition_F2 : public composite
		{
		public:
			F14_composition_F2(param_map &v);
			F14_composition_F2(const std::string &name, size_t size_var, size_t size_obj);
			virtual ~F14_composition_F2();
		protected:
			virtual void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			virtual void set_function();

		private:

		};
	}
}
#endif // !OFEC_F14_COMPOSITION_F2_H

