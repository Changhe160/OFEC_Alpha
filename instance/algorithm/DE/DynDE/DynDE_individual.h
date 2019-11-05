/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou


#ifndef OFEC_DynDE_individual_H
#define OFEC_DynDE_individual_H

#include "../individual.h"


namespace OFEC {
	class DynDE_subpopulation;
	//class DynPopDESubPop;
	class DynDE_individual final : public DE::individual
	{
		friend class DynDE_subpopulation;
		//friend class DynPopDESubPop;
	public:
		enum class individual_type { TYPE_DE, TYPE_ENTROPY_DE, TYPE_QUANTUM, TYPE_BROWNIAN };
		DynDE_individual(size_t num_obj, size_t num_con, size_t size_var) : individual(num_obj, num_con, size_var) {}

		evaluation_tag brownian(const solution_type &best, real sigma);
		evaluation_tag quantum(const solution_type &best, real r);
		evaluation_tag entropy(real sigma);
	private:
		individual_type m_type = individual_type::TYPE_DE;
	};
}
#endif // OFEC_DynDE_individual_H
