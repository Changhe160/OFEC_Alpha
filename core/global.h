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
#include <set>
#include "../utility/random/newran.h"
#include "problem/problem.h"
#include "algorithm/algorithm.h"
#include "../utility/factory.h"
#include <thread>

namespace OFEC{

	struct global{
		global(const int runID, real seed_pro, real seed_alg);

		const int m_runID;

		std::unique_ptr<problem> m_problem;
		std::unique_ptr<algorithm> m_algorithm;

		std::mutex m_mutex;
		std::map<caller, std::unique_ptr<uniform>> m_uniform;
		std::map<caller, std::unique_ptr<normal>> m_normal;
		std::map<caller, std::unique_ptr<cauchy>> m_cauchy;
		std::map<caller, std::unique_ptr<levy>> m_levy;
		std::map<caller, std::unique_ptr<gamma>> m_gamma;

        static thread_local std::shared_ptr<global> ms_global;

		static param_map ms_arg;
		static std::map<const std::string, bool> ms_filename_info; // to indicate whether a const std::string information is included in output file's name 
		static std::map<const std::string, const std::string> ms_param;
		static factory<problem> ms_reg_problem;
		static factory<algorithm> ms_reg_algorithm;
		static size_t ms_sample_fre;
		static std::map<std::string, std::set<std::string>> ms_alg4pro;
	};
}
#endif // !OFEC_GLOBAL_H

