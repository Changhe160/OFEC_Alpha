
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao
* Email: diaoyiyacug@gmail.com
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
* some general methods used in ofec are defined in this file, including distance
* metrics, solution domination relationship,
*
*********************************************************************************/

#ifndef OFEC_CUSTOM_FINCTIONAL_H
#define OFEC_CUSTOM_FINCTIONAL_H

#include<string>
#include<vector>
#include<functional>
#include<array>
#include<thread>
#include<sstream>
#include <map>
#include <fstream>
#include "../../core/random/newran.h"
#include "../../core/problem/solution.h"
#include "../../core/environment/environment.h"
#include <filesystem>

namespace UTILITY {

	extern size_t getCurSize(double scale, size_t size);
	extern bool judgeEqualWithPrecision(double a, double b, double pre);
	extern void idx2RealIdxs(int idx,
		std::array<int, 3>& idxs,
		const std::array<int, 3>& divs);

	// multithreads 
	extern void assignThreads(int num_samples, int num_task, std::vector<int>& ids);
	extern void generateRandomSolutionsMultiThreads(
		std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
		ofec::Environment* env,
		ofec::Random* rnd
	);

	
	extern void generateRandomSolutionsMultiThreads(
		std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
		ofec::Random *rnd,
		ofec::Environment *env,
		const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
	);
	
	extern void evaluateRandomSolutionsMultiThreads(
		std::vector<ofec::SolutionBase*>& samples,
		ofec::Environment *env,
		const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
	);

	// multi process
	bool waitForFile(const std::string& saveDir, const std::string& filename, const std::string& process_name);
	


	extern void getPrimeNumber(int maxNumber, std::vector<int>& primeNumber);

	std::vector<double> valuesToSortedValues(const std::vector<double>& values);

}

#endif