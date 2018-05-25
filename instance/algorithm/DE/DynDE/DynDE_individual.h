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
	namespace DE {
		class DynDE_subpopulation;
		//class DynPopDESubPop;
		class DynDE_individual : public individual
		{
			friend class DynDE_subpopulation;
			//friend class DynPopDESubPop;
		public:
			enum class individual_type { TYPE_DE, TYPE_ENTROPY_DE, TYPE_QUANTUM, TYPE_BROWNIAN };
			template<typename ... Args>
			DynDE_individual(size_t no, Args&& ... args) : individual(no, std::forward<Args>(args)...) {}
			DynDE_individual(const DynDE_individual & indi);

			evaluation_tag brownian(const solution_type &best, double sigma);
			evaluation_tag quantum(const solution_type &best, double r);
			evaluation_tag entropy(double sigma);
		private:
			individual_type m_type;
		};
	}
}
#endif // OFEC_DynDE_individual_H
