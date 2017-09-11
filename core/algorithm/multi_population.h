/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class multi_population manages a set of populations with the same type of individuals.
*
*********************************************************************************/
#ifndef OFEC_MULTI_POPULATION_H
#define OFEC_MULTI_POPULATION_H

#include "population.h"

namespace OFEC {
	
	template<typename Population>
	class multi_population
	{		
	public:
		using iterator_type = typename std::vector<std::unique_ptr<Population>>::iterator;

		multi_population() = default;
		virtual ~multi_population() {}

		multi_population(int n) :m_sub(n) {}
		multi_population(int n,int subsize) :m_sub(n, new Population(subsize)) {}
		
		void resize_objective(int n);
		void resize_variable(int n);

		iterator_type operator +(Population&& p);
		iterator_type operator +(Population& p);
		void operator- (int id);

		Population& operator[](int i);
		const Population& operator[](int i) const;

		void handle_evaluation_tag();

	protected:
		std::vector<std::unique_ptr<Population>> m_sub;
		int m_maxsize;		// the maximum size of each sub-population
	};
	

}
#endif // !OFEC_MULTI_POPULATION_H

