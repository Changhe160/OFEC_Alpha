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
*********************************************************************************/
// updated Jan 19, 2019 by Li Zhou

#ifndef OFEC_DYNSAMULTIPOP
#define OFEC_DYNSAMULTIPOP

#include "../../../../../core/algorithm/multi_population.h"
#include "../../../../problem/realworld/EPANET/epanet.h"

namespace OFEC {

	template <typename Population>
	class DynSaMultiPop : public multi_population<Population>
	{
		using PAIR = std::pair<variable_epanet, real>;
		struct CmpByValue {
			bool operator()(const PAIR& lhs, const PAIR& rhs) {
				return lhs.second < rhs.second;
			}
		};
 	public:
		DynSaMultiPop(size_t n, size_t subsize): multi_population<Population>(n,subsize){}
		void population_competition(size_t indx1, size_t indx2);
		bool is_high_coverage_rate(size_t indx1, size_t indx2, double maxrate);
		bool is_stable();
		void add_new_population(size_t num, init_type type);
	protected:
		std::vector<size_t> m_count_last;
	};

	template <typename Population>
	void DynSaMultiPop<Population>::population_competition(size_t indx1, size_t indx2) {
		
		size_t sub_size1 = multi_population<Population>::m_sub[0]->m_first_pop->size();
		std::vector<std::pair<variable_epanet, real>> temp1;
		for (size_t i = 0; i < sub_size1; ++i)
			temp1.push_back(std::make_pair((*(multi_population<Population>::m_sub[indx1]->m_first_pop))[i].variable(), (*(multi_population<Population>::m_sub[indx1]->m_first_pop))[i].objective(0)));
		for (size_t i = 0; i < sub_size1; ++i)
			temp1.push_back(std::make_pair((*(multi_population<Population>::m_sub[indx2]->m_first_pop))[i].variable(), (*(multi_population<Population>::m_sub[indx2]->m_first_pop))[i].objective(0)));
		sort(temp1.begin(), temp1.end(), CmpByValue());
		for (size_t i = 0; i < sub_size1; ++i) {
			(*(multi_population<Population>::m_sub[indx1]->m_first_pop))[i].variable() = temp1[i].first;
			(*(multi_population<Population>::m_sub[indx1]->m_first_pop))[i].objective()[0] = temp1[i].second;
		}

		size_t sub_size2 = multi_population<Population>::m_sub[0]->m_second_pop->size();
		std::vector<std::pair<variable_epanet, real>> temp2;
		for (size_t i = 0; i < sub_size2; ++i)
			temp2.push_back(std::make_pair((*(multi_population<Population>::m_sub[indx1]->m_second_pop))[i].variable(), (*(multi_population<Population>::m_sub[indx1]->m_second_pop))[i].objective(0)));
		for (size_t i = 0; i < sub_size2; ++i)
			temp2.push_back(std::make_pair((*(multi_population<Population>::m_sub[indx2]->m_second_pop))[i].variable(), (*(multi_population<Population>::m_sub[indx2]->m_second_pop))[i].objective(0)));
		sort(temp2.begin(), temp2.end(), CmpByValue());
		for (size_t i = 0; i < sub_size2; ++i) {
			(*(multi_population<Population>::m_sub[indx1]->m_second_pop))[i].variable() = temp2[i].first;
			(*(multi_population<Population>::m_sub[indx1]->m_second_pop))[i].objective()[0] = temp2[i].second;
		}

	}

	template <typename Population>
	bool DynSaMultiPop<Population>::is_high_coverage_rate(size_t indx1, size_t indx2, double maxrate) {
		int size1 = multi_population<Population>::m_sub[indx1]->m_first_pop->size();
		int size2 = multi_population<Population>::m_sub[indx2]->m_first_pop->size();
		std::vector<bool> flag(size2, true);
		int count = 0;
		for (size_t i = 0; i < size1; ++i) {
			for (size_t j = 0; j < size2; ++j) {
				if (flag[j] && (*(multi_population<Population>::m_sub[indx1]->m_first_pop))[i].variable().index() == (*(multi_population<Population>::m_sub[indx2]->m_first_pop))[j].variable().index()) {
					++count;
					flag[j] = false;
					break;
				}
			}
		}
		double rate = (double)count / (double)size1;
		return rate > maxrate;
	}

	template <typename Population>
	bool DynSaMultiPop<Population>::is_stable() {
		size_t node_size = CAST_RP_EPANET->number_node();
		std::vector<size_t> count(node_size, 0);
		int num_pop = multi_population<Population>::size();
		for (size_t i = 0; i < num_pop; ++i) {
			std::vector<bool> flag(node_size, false);
			for (size_t j = 0; j < multi_population<Population>::m_sub[i]->m_first_pop->size(); ++j) {
				size_t temp = (*(multi_population<Population>::m_sub[i]->m_first_pop))[j].variable().index() - 1;
				if (!flag[temp]) {
					++count[temp];
					flag[temp] = true;
				}
			}
		}
		// compute coverage rate

		/*int sum = 0;
		for (size_t i = 0; i < node_size; ++i)
		sum += abs((int)count[i] - 1);*/

		/*int count_standard = 0;
		for (auto &i : count) {
		if (i > 1)  ++count_standard;
		}
		int standard = num_pop / 2;
		int count_num = 0;
		for (size_t i = 0; i < node_size; ++i) {
		if (count[i] >= standard)
		++count_num;
		}*/
		if (m_count_last.size() == 0) {
			m_count_last = count;
			return false;
		}
		for (size_t i = 0; i < count.size(); ++i) {
			if (m_count_last[i] != count[i]) {
				m_count_last = count;
				return false;
			}

		}
		return true;


		/*if (count_num >= count_standard/2) return true;
		else return false;*/
	}

	template <typename Population>
	void DynSaMultiPop<Population>::add_new_population(size_t num, init_type type) {
		size_t popsize = multi_population<Population>::m_sub[0]->m_first_pop->size();
		size_t flag = multi_population<Population>::size();
		for (size_t i = 0; i < num; ++i)
			multi_population<Population>::m_sub.emplace_back(std::move(std::unique_ptr<Population>(new Population(popsize))));
		
		if (type == init_type::random)
			CAST_RP_EPANET->set_init_type(init_type::random);
		else if (type == init_type::be_visited) {
			CAST_RP_EPANET->set_init_type(init_type::be_visited);
			CAST_RP_EPANET->cal_pro_by_be_visited();
		}
		else if (type == init_type::distance)
			CAST_RP_EPANET->set_init_type(init_type::distance);
		else throw myexcept("No this initialization type");
		for (size_t i = flag; i < flag + num; ++i) {
			CAST_RP_EPANET->m_pop_identifier = i - flag;
			
			multi_population<Population>::m_sub[i]->initialize_new_pop();
		}
	}


}

#endif