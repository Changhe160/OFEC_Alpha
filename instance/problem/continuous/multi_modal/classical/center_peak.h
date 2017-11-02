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
******************************************************************************************
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#ifndef OFEC_CENTER_PEAK_H
#define OFEC_CENTER_PEAK_H

#include "../../../../../core/problem/continuous/function.h"
//Ursem F4 in R. K. Ursem, ¡°Multinational evolutionary algorithms,¡± in Proc. Congr.
//Evol. Comput. (CEC), vol. 3. 1999, pp. 1633¨C1640.
namespace OFEC {
	
	class center_peak : public function
	{//Ursem F4
	public:
		center_peak(param_map &v);
		center_peak(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}
#endif // !OFEC_CENTER_PEAK_H