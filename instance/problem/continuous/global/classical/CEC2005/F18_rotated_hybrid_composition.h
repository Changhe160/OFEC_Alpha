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

#ifndef OFEC_F18_ROTATED_HYBRID_COMPOSITION_H
#define OFEC_F18_ROTATED_HYBRID_COMPOSITION_H

#include "F15_hybrid_composition.h"

namespace OFEC {

	class F18_rotated_hybrid_composition : public F15_hybrid_composition
	{
	public:
		F18_rotated_hybrid_composition(param_map &v);
		F18_rotated_hybrid_composition(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F18_rotated_hybrid_composition();
	protected:
		virtual void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
		virtual void set_function();

		virtual void set_translation();
		virtual void set_rotation();
	private:

	};
}
#endif // !OFEC_F18_ROTATED_HYBRID_COMPOSITION_H
