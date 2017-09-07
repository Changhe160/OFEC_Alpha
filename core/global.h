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
* class globla defines global objects used in OFEC
*
*
*********************************************************************************/
#ifndef OFEC_GLOBAL_H
#define OFEC_GLOBAL_H

#include <mutex>

#include "../utility/random/newran.h"
#include "problem/problem.h"
#include "definition.h"
#include "../utility/typevar/typevar.h"

namespace OFEC{

#define GET_NUM_DIM (global::ms_global->m_problem->variable_size())
#define GET_NUM_OBJ (global::ms_global->m_problem->objective_size())
#define GET_EVALS (global::ms_global->m_problem->m_effective_eval())

#ifdef USING_GPU
	using compute = boost::compute;
#endif // USING_GPU

	struct global{
		std::unique_ptr<problem> m_problem;
		std::unique_ptr<cauchy> m_cauchyPro;							// cauchy random number for algorithm
		std::unique_ptr<normal> m_normalPro;							// gaussian random number for algorithm
		std::unique_ptr<levy> m_levyPro;
		std::unique_ptr<cauchy> m_cauchyAlg;							// cauchy random number for algorithm
		std::unique_ptr<normal> m_normalAlg;							// gaussian random number for algorithm
		std::unique_ptr<levy> m_levyAlg;
		std::unique_ptr<uniform> m_uniformPro;							// random number of uniform distribution for algorithm
		std::unique_ptr<uniform> m_uniformAlg;							// random number of uniform distribution for algorithm
		
		//std::unique_ptr<algorithm> m_algorithm;

		int global::getRandInt(const int min, const int max, caller mode) {
			if (min == max || min == max - 1) return min;
			if (mode == caller::Algorithm)	return static_cast<int>(min + (max - min)*m_uniformAlg->next());
			else	return static_cast<int>(min + (max - min)*m_uniformPro->next());
		}
		double global::getRandFloat(const double min, const double max, caller mode) {
			if (min == max) return min;
			if (mode == caller::Algorithm)	return min + (max - min)*m_uniformAlg->next();
			else		return min + (max - min)*m_uniformPro->next();
		}

		std::mutex m_mutex, m_stream_mutex;

#ifdef OFEC_CONSOLE
		static thread_local std::unique_ptr<global> ms_global;
#endif // OFEC_CONSOLE

#ifdef OFEC_DEMON
		static unique_ptr<global> ms_global;
#endif
		
#ifdef USING_GPU
		static compute::device m_device;
		compute::context context;
#endif // USING_GPU
		static param_map ms_arg;
	};
}
#endif // !OFEC_GLOBAL_H

