/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Operator adaptator of genetic learning algorithm
*
*********************************************************************************/


#ifndef OPERATOR_ADAPTOR_H
#define OPERATOR_ADAPTOR_H
#include <vector>
#include <memory>
#include "../../../../core/definition.h"
namespace OFEC {
	namespace EDA {
		template<typename Individual>
		class adaptor {
		protected:
		public:
			adaptor() = default;
			virtual ~adaptor() {}
			virtual void update_probability(std::vector<std::unique_ptr<Individual>> &pop, const std::vector<double>& weight, const std::vector<int> *index = nullptr) = 0;
			virtual void update_probability(std::vector<Individual> &pop, const std::vector<double>& weight, const std::vector<int> *index = nullptr) = 0;
			virtual void create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, double alpha) = 0;
			virtual void create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, std::vector<Individual> &his, double alpha) {};
			virtual evaluation_tag update_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, int& impRadio, int ms) = 0;
			virtual void print_infor() {};
		};
	}
}
#endif //OPERATOR_ADAPTOR_H