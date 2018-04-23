
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

#include "rotated_bent_cigar.h"
namespace OFEC {

	rotated_bent_cigar::rotated_bent_cigar(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
		bent_cigar((v.at("proName")), (v.at("numDim")), 1) {
		initialize();
	}
	rotated_bent_cigar::rotated_bent_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		bent_cigar(name, size_var, 1) {
		initialize();
	}

	void rotated_bent_cigar::initialize() {

		load_rotation("instance/problem/continuous/global/CEC2005/data/");
		m_optima.clear();
		set_global_opt();
	}

	void rotated_bent_cigar::evaluate__(real *x, std::vector<real>& obj) {
		bent_cigar::evaluate__(x, obj);

	}

}