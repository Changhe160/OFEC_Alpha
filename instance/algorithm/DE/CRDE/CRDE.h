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
// updated on Mar 28, 2018 by Li Zhou
// Updated on 15th Aug, 2019 by Junchen Wang

/*
Thomsen R (2004) Multimodal optimization using crowding-based differential evolution. In: Proceedings of
the 2004 IEEE congress on evolutionary computation, pp 1382�C1389. IEEE Press, Portland, OR, USA
*/
#ifndef OFEC_CRDE_H
#define OFEC_CRDE_H

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {

	class CRDE_pop final : public DE::population<DE::individual> {
	public:
		explicit CRDE_pop(size_t size_pop);
		evaluation_tag evolve() override;
	};

	class CRDE final : public algorithm {
	public:
		explicit CRDE(param_map &v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
		CRDE_pop m_pop;
	};
}
#endif // OFEC_CRDE_H
