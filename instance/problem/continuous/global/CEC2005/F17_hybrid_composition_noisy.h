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

#ifndef OFEC_F17_HYBRID_COMPOSITION_NOISY_H
#define OFEC_F17_HYBRID_COMPOSITION_NOISY_H

#include "F16_rotated_hybrid_composition_F15.h"

namespace OFEC {
	namespace CEC2005 {
		class F17_hybrid_composition_noisy : public F16_rotated_hybrid_composition_F15
		{
		public:
			F17_hybrid_composition_noisy(param_map &v);
			F17_hybrid_composition_noisy(const std::string &name, size_t size_var, size_t size_obj);
			virtual ~F17_hybrid_composition_noisy();
		protected:
			virtual void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			double noise();
		private:

		};
	}
}
#endif // !OFEC_F17_HYBRID_COMPOSITION_NOISY_H
