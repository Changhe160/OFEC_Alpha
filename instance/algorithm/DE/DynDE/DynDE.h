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
/*
Mendes, R., Mohais, A.: DynDE: a differential evolution for dynamic optimization problems. In: Proceedings
of the IEEE Congress on Evolutionary Computation (CEC¡¯05), pp. 2808¨C2815. IEEE (2005)
*/
#ifndef OFEC_DYNDE_H
#define OFEC_DYNDE_H

#include "DynDESubPop.h"
#include "../../../../core/algorithm/multi_population.h"

namespace OFEC {
	class DynDE : public DEpopulation<DynDEindividual>, public multi_population<DynDESubPop>
	{
	public:
		DynDE(param_map &v);
		void set_default(param_map &v);
		evaluation_tag run_();
		void exclude();
	public:
		int m_M;        // the number of populations
		double m_r_excl; // radius of exlusion radius

	};
}
#endif // OFEC_DYNDE_H
