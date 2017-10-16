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

#ifndef OFEC_F25_ROTATED_HYBRID_NO_BOUND_H
#define OFEC_F25_ROTATED_HYBRID_NO_BOUND_H

#include "F24_rotated_hybrid_composition.h"

namespace OFEC {

	class F25_rotated_hybrid_no_bound : public F24_rotated_hybrid_composition
	{
	public:
		F25_rotated_hybrid_no_bound(param_map &v);
		F25_rotated_hybrid_no_bound(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F25_rotated_hybrid_no_bound();
	protected:
		virtual void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
		
		void set_translation();
	private:

	};
}
#endif // !OFEC_F25_ROTATED_HYBRID_NO_BOUND_H

