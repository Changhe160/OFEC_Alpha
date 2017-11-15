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


#include "hybrid.h"
namespace OFEC {
	namespace CEC2015 {
		hybrid::hybrid(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		continuous(name, size_var, size_obj), m_random_perm(size_var)
		{
		}
		hybrid::~hybrid() {
			//dtor
			if (m_num_function > 0) {
				for (auto &i : m_function)
					if (i) delete i;
			}
		}

		size_t hybrid::get_num_function() {
			return m_num_function;
		}

	
		void hybrid::evaluate__(real *x, std::vector<real>& obj) {
			size_t count = 0;
			
			for (size_t i = 0; i < m_num_function; ++i) {
				variable<real> temp_var(m_dim[i]);
				objective<real> temp_obj(m_objective_size);
				for (size_t j = 0; j < m_dim[i]; ++j)
					temp_var[j] = x[m_random_perm[count++]];
				//std::copy(x + m_start[i], x + m_start[i] + m_dim[i], temp_var.begin());
				solution<variable<real>, real> temp(std::move(temp_var), std::move(temp_obj));
				m_function[i]->evaluate_(temp, caller::Problem, false, false);
				obj[0] += temp.get_objective()[0];
			}
		}

		function* hybrid::get_function(size_t num) {
			return m_function[num];
		}
		
	}
}

