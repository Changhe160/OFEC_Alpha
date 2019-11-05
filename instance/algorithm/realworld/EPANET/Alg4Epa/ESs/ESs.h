/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
* Zechman, Emily M.£¬ Ranjithan, S. Ranji. Evolutionary Computation-Based 
* Methods for Characterizing Contaminant Sources in a Water Distribution System[J].
* Journal of Water Resources Planning and Management. 2009, 135(5): 334-343.
*********************************************************************************/
// updated Apr 12, 2019 by Li Zhou


#ifndef OFEC_ESS_H
#define OFEC_ESS_H

#include "ESs_population.h"
#include "../../../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class ESs : public algorithm {
	public:
		ESs(param_map & v);
		void run_();
		void initialize();
		void record();
	private:
		ESs_population m_pop;
		const std::string m_path_result;
		std::vector<real> m_coverage_result;
		std::vector<real> m_curr_best_obj;
	private:
		real cal_coverage_rate();
		void record_intermediate_result();
	};

}
#endif // OFEC_ESS_H

