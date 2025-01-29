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

#include "sphere.h"

namespace ofec {
	void Sphere::addInputParameters() {
		m_input_parameters.add("number of variables", new RangedSizeT(m_number_variables, 1, 1000, 2));
	}

	void Sphere::initialize_(Environment *env) {
		Function::initialize_(env);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
		resizeVariable(m_number_variables);
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
	}

	void Sphere::updateOptima(Environment *env) {
		setGlobalOpt();
		m_objective_accuracy = 1e-8;
	}

	void Sphere::evaluateOriginalObj(Real *x, std::vector<Real> &obj) const {
		Real fit = 0;
		for (int i = 0; i < m_number_variables; i++) {
			fit += x[i] * x[i];
		}
		obj[0] = fit + m_bias;
	}
	
}