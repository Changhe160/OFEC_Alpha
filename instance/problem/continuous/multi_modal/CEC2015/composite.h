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

#ifndef OFEC_COMPOSITE_H
#define OFEC_COMPOSITE_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2015 {
		class composite : public function
		{
		public:
			composite(const std::string &name, size_t size_var, size_t size_obj);
			~composite();
			size_t get_num_function();
			function* get_function(size_t num);
		protected:
			virtual void initialize() = 0;
			void evaluate__(real *x, std::vector<real>& obj);
		
			virtual void set_function() = 0;
			virtual void set_weight(std::vector<double>& w, const std::vector<real>&x);

			bool load_translation(const string &path);
			void set_translation();
			bool load_rotation(const string &path);
			void set_rotation();
		protected:
			size_t m_num_function;                // number of basic functions, for hybrid functions
			std::vector<function*> m_function;    // the functions
			std::vector<real> m_height;
			std::vector<double> m_converge_severity;     // severity of converge range for each function
			std::vector<real> m_f_bias;
		};

	}
}
#endif // ! OFEC_COMPOSITE_H
