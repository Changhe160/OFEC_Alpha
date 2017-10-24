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

#ifndef OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H
#define OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H

#include "F21_rotated_hybrid_composition.h"

namespace OFEC {
	namespace CEC2005 {
		class F22_rotated_hybrid_high_condition_number_matrix : public F21_rotated_hybrid_composition
		{
		public:
			F22_rotated_hybrid_high_condition_number_matrix(param_map &v);
			F22_rotated_hybrid_high_condition_number_matrix(const std::string &name, size_t size_var, size_t size_obj);
			virtual ~F22_rotated_hybrid_high_condition_number_matrix();
		protected:
			virtual void initialize();
			void evaluate__(real *x, std::vector<real>& obj);

		private:

		};
	}
}
#endif // !OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H
