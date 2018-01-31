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


#ifndef OFEC_DYNDEINDIVIDUAL_H
#define OFEC_DYNDEINDIVIDUAL_H

#include "../DEindividual.h"


namespace OFEC {
	class DynDESubPop;
	class DynPopDESubPop;
	class DynDEindividual : public DEindividual<>
	{
		friend class DynDESubPop;
		friend class DynPopDESubPop;
	public:
		enum class IndividualType { TYPE_DE , TYPE_ENTROPY_DE, TYPE_QUANTUM, TYPE_BROWNIAN };
		template<typename ... Args>
		DynDEindividual(size_t no, Args&& ... args) : DEindividual(no, std::forward<Args>(args)...) {}
		DynDEindividual(const DynDEindividual & indi);
		
		evaluation_tag brownian(const solution_type &best, double sigma);
		evaluation_tag quantum(const solution_type &best, double r);
		evaluation_tag entropy(double sigma);
	private:
		IndividualType m_type;
	};
}
#endif // OFEC_DYNDEINDIVIDUAL_H
