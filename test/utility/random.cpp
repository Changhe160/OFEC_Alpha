/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li & Xia Hai
* Email: changhe.lw@gmail.com
* Language: C++
* Last-modified date: 2018.08.17
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Pseudo-random number Generator test file; 
*
*********************************************************************************/

#include "../utility/catch.hpp"
#include "../utility/random/newran.h"
#include <iomanip>

using namespace ofec;

TEST_CASE("Test normal distribution random generator", "[random][normal]") {
	Random rand(0.5);
	std::cout << std::setprecision(5);
	rand.initialize();
	for (size_t i = 0; i < 10; ++i)
		std::cout << rand.normal.next() << std::endl;
	std::cout << std::endl;
	rand.initialize();
	for (size_t i = 0; i < 10; ++i)
		std::cout << rand.normal.next() << std::endl;
}

//TEST_CASE("PRNG OS test", "[random][OS]") {
//	Uniform uni(0.5);
//	for (size_t i = 0; i < 10; i++) {
//		std::cout << uni.next() << " ";
//	}
//	std::cout << std::endl;
//}
//
//TEST_CASE("PRNG uniform test", "[random][uniform]") {
//	std::vector<int> times{ 1000,10000,100000,1000000,10000000 };
//	//totaltimes[0],[1],[2],[3] conrespond to 1000,10000,100000,100000;
//	std::vector<std::map<int, long int>> totaltimes2numcounter;
//	for (int times_idx = 0; times_idx < 5; ++times_idx) {
//		int totoltimes = times[times_idx];
//		std::map<int, long int>counter;
//		for (int seed_idx = 1; seed_idx < 11; ++seed_idx) {
//			double seed = (double)seed_idx*0.08;
//			Uniform uni(seed);
//			//global::ms_global.reset(new global(0, seed, seed));
//			for (int i = 0; i < totoltimes; ++i) {
//				++counter[uni.nextNonStd(1, 11)];
//			}
//		}
//		for (auto & num : counter) {
//			num.second /= 10;
//		}
//		totaltimes2numcounter.emplace_back(counter);
//	}
//	std::vector<float> ratio;
//
//	for (int i = 0; i < times.size(); ++i) {
//		int mu = times[i] / 10;
//		int sigma_all = 0;
//		for (int num = 1; num < 11; ++num) {
//			sigma_all += std::abs(totaltimes2numcounter[i][num] - mu);
//		}
//		ratio.emplace_back((float)sigma_all / (float)times[i]);
//	}
//	std::cout << "Output the uniform sampling result from 1 to 10" << std::endl << std::endl;
//	std::cout <<std::setw(15)<< "num\\totoal times" << std::setw(15) << "1000" << std::setw(15)<< "10000" <<std::setw(15) << "100000" << std::setw(15)<<
//		"1000000"<< std::setw(15)<<"10000000" << std::endl;
//	for (int num = 1; num < 11; ++num) {
//		std::cout << std::setw(15)<< num;
//		for (int j = 0; j < totaltimes2numcounter.size(); ++j) {
//			std::cout<<std::setw(15)<< totaltimes2numcounter[j][num];
//		}
//		std::cout << std::endl;
//	}
//	std::cout << std::setw(15)<< "deviation ratio";
//	for (int i = 0; i < 5; ++i) {
//		std::cout <<std::setw(15)<< ratio[i];
//	}
//	std::cout << std::endl;
//	std::cout << "PRNG_uniform test finishing";
//}
//
//TEST_CASE("PRNG normal test", "[random][normal]") {
//	std::vector<int> times{ 1000,10000,100000,1000000,10000000 };
//	//mean and variance pair for various sampling numbers;
//	std::vector < std::pair<double, double>> result;
//	//totaltimes[0],[1],[2],[3] conrespond to 1000,10000,100000,100000;
//	std::vector<std::map<int, long int>> totaltimes2numcounter;
//	for (int times_idx = 0; times_idx < 5; ++times_idx) {
//		int totoltimes = times[times_idx];
//		double mean = 0.;
//		double variance = 0.;
//		for (int seed_idx = 1; seed_idx < 11; ++seed_idx) {
//			std::vector<Real> random_num_seq;
//			double seed = (double)seed_idx*0.08;
//			Normal nor(seed);
//			for (int i = 0; i < totoltimes; ++i) {
//				random_num_seq.push_back(nor.next());
//			}
//			double sum = 0.;
//			for (auto & num : random_num_seq) {
//				sum += num;
//			}
//			//for one seed;
//			double temp_mean = sum / random_num_seq.size();
//			double temp_variance = 0.;
//			for (auto & num : random_num_seq) {
//				temp_variance += std::pow(std::fabs(num - temp_mean), 2.0);
//			}
//			temp_variance /= (random_num_seq.size() - 1.);
//			mean += temp_mean;
//			variance += temp_variance;
//		}
//		//for ten seeds on average;
//		mean /= 10.;
//		variance /= 10.;
//		result.emplace_back(mean, variance);
//	}
//	std::cout << "Output the sampling result for normal distribution with mean 0, variance 1 repectively" << std::endl << std::endl;
//	std::cout <<std::setw(20)<< "totoal times" <<std::setw(20)<< "1000" << std::setw(20)<< "10000" << std::setw(20)<< "100000" << std::setw(20)<<
//		"1000000"<< std::setw(20)<<"10000000" << std::endl;
//	std::cout << std::setw(20)<< "sampling mean";
//	for (const auto &i : result) {
//		std::cout << std::setw(20) << i.first;
//	}
//	std::cout << std::endl;
//	std::cout << std::setw(20)<< "sampling variance";
//	for (const auto & i : result) {
//		std::cout << std::setw(20) << i.second;
//	}
//	std::cout << std::endl;
//	std::cout << "PRNG_normal test finishing";
//}