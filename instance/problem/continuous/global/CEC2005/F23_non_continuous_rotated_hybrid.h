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

#ifndef OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H
#define OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H

#include "composition.h"

namespace OFEC {
	namespace CEC2005 {
		class F23_non_continuous_rotated_hybrid final : public composition
		{
		public:
			F23_non_continuous_rotated_hybrid(param_map &v);
			F23_non_continuous_rotated_hybrid(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
		private:

		};
	}
	using CEC2005_GOP_F23 = CEC2005::F23_non_continuous_rotated_hybrid;
}
#endif // !OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H
