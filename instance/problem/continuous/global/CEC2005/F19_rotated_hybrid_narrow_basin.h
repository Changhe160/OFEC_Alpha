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

#ifndef OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H
#define OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H

#include "composition.h"

namespace OFEC {
	namespace CEC2005 {
		class F19_rotated_hybrid_narrow_basin final : public composition
		{
		public:
			F19_rotated_hybrid_narrow_basin(param_map &v);
			F19_rotated_hybrid_narrow_basin(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);

			void set_function();
			void set_translation();
		private:

		};
	}
}
#endif // ! OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H

