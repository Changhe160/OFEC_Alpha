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

#ifdef USING_GPU
	using compute = boost::compute;
#endif // USING_GPU

	struct global{
		//global();

		std::unique_ptr<problem> m_problem;
		//std::unique_ptr<algorithm> m_algorithm;

		std::mutex m_mutex, m_stream_mutex;
		std::map<caller, std::unique_ptr<uniform>> m_uniform;
		std::map<caller, std::unique_ptr<normal>> m_normal;
		std::map<caller, std::unique_ptr<cauchy>> m_cauchy;
		std::map<caller, std::unique_ptr<levy>> m_levy;
		std::map<caller, std::unique_ptr<gamma>> m_gamma;

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

