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

#ifndef OFEC_CEC2005_COMPOSITION_H
#define OFEC_CEC2005_COMPOSITION_H

#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2005 {
		class composition : public continuous
		{
		public:
			composition(const std::string &name, size_t size_var, size_t size_obj);
			~composition();
			size_t get_num_function();
			function* get_function(size_t num);
			
		protected:
			virtual void initialize() = 0;
			virtual void evaluate__(real *x, std::vector<real>& obj);
			void compute_fmax();
			virtual void set_function() = 0;
			virtual void set_weight(std::vector<double>& w, const std::vector<real>&x);

			virtual bool load_translation(const std::string &path);
			virtual void set_translation();
			virtual bool load_rotation(const std::string &path);
			virtual void set_rotation();
		protected:
			size_t m_num_function;                // number of basic functions, for hybrid functions
			std::vector<function*> m_function;    // the functions
			std::vector<real> m_height;
			std::vector<real> m_fmax;
			double m_height_normalize_severity;   // constant number for noralizing all basic function with similar height
			std::vector<double> m_converge_severity;     // severity of converge range for each function
			std::vector<double> m_stretch_severity;      // severity of stretching original function, greater than 1 for stretch

		};
		
	}
}
#endif // ! OFEC_CEC2005_COMPOSITION_H
