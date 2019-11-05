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


#ifndef OFEC_DYNDESUBPOP_H
#define OFEC_DYNDESUBPOP_H
#include "DynDE_individual.h"
#include "../population.h"
namespace OFEC {
	//class DynDE;
	class DynDE_subpopulation final : public DE::population<DynDE_individual> {
	public:
		//friend class DynDE;
		explicit DynDE_subpopulation(size_t size);
		evaluation_tag evolve() override;
		bool get_flag() const { return m_flag; }
		void set_flag(bool flag) { m_flag = flag; }
	protected:
		void assign_type();
	protected:
		bool m_flag; // wheter need to be re-initialize, marked in the exclution_check()
		int m_num_normal;    // the number of normal individuals of each swarm
		int m_num_brownian;       // the number of brownian individuals of each swarm
		int m_num_quantum;    // the number of quantum individuals of each swarm
		real m_r_cloud; // radius of quantum swarms
		real m_sigma;	// deviation for generation of Brownian individuals
	};
}
#endif //OFEC_DYNDESUBPOP_H