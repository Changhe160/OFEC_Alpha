
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

#include "rotated_rosenbrock.h"
namespace OFEC {

	rotated_rosenbrock::rotated_rosenbrock(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
		rosenbrock((v.at("proName")), (v.at("numDim")), 1) {
		
	}
	rotated_rosenbrock::rotated_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		rosenbrock(name, size_var, 1) {
		
	}

	void rotated_rosenbrock::initialize() {
		set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::ConOP }));
		m_variable_monitor = true;
		set_range(-2.048, 2.048);
		set_init_range(-2.048, 2.048);
		std::vector<double> v(m_variable_size, 1);
		set_original_global_opt(v.data());

		set_condition_number(1);
		load_rotation("instance/problem/continuous/global/CEC2005/data/");
	
		set_global_opt();
	}

	void rotated_rosenbrock::evaluate__(real *x, std::vector<real>& obj) {
		rosenbrock::evaluate__(x, obj);

	}

}