/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015
*  Competition on Learning-based Real-Parameter Single Objective
*  Optimization.
************************************************************************/
#ifndef OFEC_F2_R_CIGAR_H
#define OFEC_F2_R_CIGAR_H

#include "../classical/bent_cigar.h"
namespace OFEC {
	namespace CEC2015 {
		class F2_R_cigar final : public bent_cigar
		{
		public:
			F2_R_cigar(param_map &v);
			F2_R_cigar(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);

		private:
		};
	}
	using CEC2015_GOP_F2 = CEC2015::F2_R_cigar;
}

#endif // !OFEC_F2_R_CIGAR_H


