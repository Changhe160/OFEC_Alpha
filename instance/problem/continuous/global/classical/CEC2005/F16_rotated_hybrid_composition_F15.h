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

#ifndef OFEC_F16_ROTATED_HYBRID_COMPOSITION_F15_H
#define OFEC_F16_ROTATED_HYBRID_COMPOSITION_F15_H

#include "F15_hybrid_composition.h"

namespace OFEC {

	class F16_rotated_hybrid_composition_F15 : public F15_hybrid_composition
	{
	public:
		F16_rotated_hybrid_composition_F15(param_map &v);
		F16_rotated_hybrid_composition_F15(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F16_rotated_hybrid_composition_F15();
	protected:
		virtual void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
		
		virtual void set_rotation();
	private:

	};
}
#endif // !OFEC_F16_ROTATED_HYBRID_COMPOSITION_F15_H
