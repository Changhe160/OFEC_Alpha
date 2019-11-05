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
* L. Liu, S. Ranji Ranjithan, G. Mahinthakumar. Contamination source identification
* in water distribution systems using an adaptive dynamic optimization procedure[J].
* Journal of Water Resources Planning and Management. 2011, 137(2): 183-192.
*********************************************************************************/
// updated Jun 10, 2019 by Li Zhou


#ifndef OFEC_ADOPT_H
#define OFEC_ADOPT_H

#include "../../../../../../core/algorithm/algorithm.h"
#include "../DynSaMultiPop.h"
#include "../CC_framework.h"

namespace OFEC {
	template <typename Population1, typename Population2, typename Individual>
	class ADOPT : public algorithm
	{
		std::mutex mutex_ADOPT_out;
		//using fill_type = CC_framework<Population1, Population2, Individual>::fill_type;

	public:
		ADOPT(param_map & v);
		void run_();
		void initialize();
	protected:
		evaluation_tag evolve();

		void update_distance(size_t idx);
		bool update_alternative_solution(size_t idx, Individual & indi);
		template <typename Population>
		bool is_feasible_population(const real tar, const Population & cc);
		real cal_standard_deviation(const std::vector<float>& vec1, const std::vector<float>& vec2);
		real cal_coverage_rate();
		void record_intermediate_result();
		void output();
		void record();
	private:
		real m_tar;
		DynSaMultiPop<CC_framework<Population1, Population2, Individual>> m_sub_population;
		std::vector<std::pair<std::unique_ptr<Individual>, size_t>> m_alternative;
		size_t m_phase_interval;
		real m_tar_multiplier;
		int m_iter = 0;
		const std::string m_path_result;

		std::vector<real> m_coverage_result;
		std::vector<size_t> m_num_pop_result;
		std::vector<real> m_curr_best_obj;

		size_t m_data_update_eval;
		size_t m_max_eval;
		size_t m_initial_phase;
		size_t m_final_phase;

	};

	//std::mutex mutex_ADOPT_out;
	template <typename Population1, typename Population2, typename Individual>
	ADOPT<Population1, Population2, Individual>::ADOPT(param_map & v) : algorithm(v.at("algName")), \
		m_sub_population((size_t)v.at("numSubPop"), (size_t)v.at("subPopSize")), m_tar((real)1.2), m_tar_multiplier((real)v.at("alpha")), \
		m_path_result(v.at("dataFileResult"))
	{
		m_max_eval = v.at("maxEvals");
		m_initial_phase = CAST_RP_EPANET->phase();
		m_final_phase = CAST_RP_EPANET->m_total_phase;
		m_data_update_eval = m_max_eval / (m_final_phase - m_initial_phase + 1);
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::initialize() {

		for (size_t i = 0; i < m_sub_population.size(); ++i) {  // initialize subpopulation 

			CAST_RP_EPANET->set_init_type(init_type::random);

			m_sub_population[i].m_first_pop->initialize();

			m_sub_population[i].m_second_pop->initialize();

			m_sub_population[i].fill_each(CC_framework<Population1, Population2, Individual>::fill_type::random);

			m_sub_population[i].update_best();

			m_alternative.emplace_back(std::make_pair(std::unique_ptr<Individual>(new Individual(*(m_sub_population[i].m_best))), i));
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::run_() {
		evaluation_tag tag = evaluation_tag::Normal;

		while (tag != evaluation_tag::Terminate) {

			//size_t total_evaluations = CAST_RP_EPANET->total_evaluations();

			record_intermediate_result();

			//std::cout << "runID: " << global::ms_global.get()->m_runID << " iteraton: " << m_iter << "  phase: " << CAST_RP_EPANET->phase() << "  total_eval: " << total_evaluations << "  alternative: " << m_alternative.size() << " coverage: " << m_coverage_result[m_iter] << std::endl;

			tag = evolve();

			size_t curr_eval = global::ms_global->m_problem->evaluations();
			if (curr_eval >= (CAST_RP_EPANET->phase() - m_initial_phase + 1) * m_data_update_eval) {

				std::vector<size_t> sub_idx;
				for (auto &i : m_alternative)
					sub_idx.emplace_back(i.second);
				m_alternative.clear();
				for (size_t i = 0; i < sub_idx.size(); ++i) {
					update_alternative_solution(sub_idx[i], *(m_sub_population[sub_idx[i]].m_best));
				}

				if (m_alternative.size() == 1) break;
				//m_tar *= 0.43;
				m_tar *= m_tar_multiplier;
				CAST_RP_EPANET->phase_next();
				if (CAST_RP_EPANET->is_time_out()) break;
			}

		}
		output();
	}

	template <typename Population1, typename Population2, typename Individual>
	evaluation_tag ADOPT<Population1, Population2, Individual>::evolve() {

		evaluation_tag tag = evaluation_tag::Normal;
		int count = 0;

		for (size_t i = 0; i < m_alternative.size(); ++i) {
			size_t idx = m_alternative[i].second;

			m_sub_population[idx].m_first_pop->update_probability();
			//m_sub_population[idx].m_second_pop->update_probability();
			m_sub_population[idx].m_second_pop->update_CR();
			m_sub_population[idx].m_second_pop->update_F();
			m_sub_population[idx].m_second_pop->update_pro_strategy();

			m_sub_population[idx].m_first_pop->mutate();
			m_sub_population[idx].m_second_pop->mutate();

			m_sub_population[idx].m_second_pop->recombine();

			/*if (is_feasible_population<GL_population>(m_tar, *(m_sub_population[idx].m_first_pop))) {
			update_distance(idx);
			m_sub_population[idx].m_first_pop->select(true);
			m_sub_population[idx].m_second_pop->select(true);
			}
			else {
			m_sub_population[idx].m_first_pop->select(false);
			m_sub_population[idx].m_second_pop->select(false);
			}*/

			/*m_sub_population[i].m_first_pop->select(false);
			m_sub_population[i].m_second_pop->select(false);*/

			bool flag_first = is_feasible_population<Population1>(m_tar, *(m_sub_population[idx].m_first_pop));
			bool flag_second = is_feasible_population<Population2>(m_tar, *(m_sub_population[idx].m_second_pop));
			if (flag_first || flag_second)
				update_distance(idx);
			m_sub_population[idx].m_first_pop->select(flag_first);
			m_sub_population[idx].m_second_pop->select(flag_second);

			m_sub_population[idx].fill_each(CC_framework<Population1, Population2, Individual>::fill_type::best);
		}
		++m_iter;
		return tag;
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::update_distance(size_t idx) {
		real min_distance;

		for (size_t j = 0; j < m_sub_population[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_sub_population.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_sub_population[idx].m_first_pop))[j].variable().index(), m_sub_population[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_sub_population[idx].m_first_pop))[j].m_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_sub_population[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_sub_population.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_sub_population[idx].m_second_pop))[j].variable().index(), m_sub_population[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_sub_population[idx].m_second_pop))[j].m_distance_fitness = min_distance;
		}

		for (size_t j = 0; j < m_sub_population[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_sub_population.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_sub_population[idx].m_first_pop))[j].trial().variable().index(), m_sub_population[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_sub_population[idx].m_first_pop))[j].m_pu_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_sub_population[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_sub_population.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_sub_population[idx].m_second_pop))[j].trial().variable().index(), m_sub_population[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_sub_population[idx].m_second_pop))[j].m_pu_distance_fitness = min_distance;
		}

	}

	template <typename Population1, typename Population2, typename Individual>
	bool ADOPT<Population1, Population2, Individual>::update_alternative_solution(size_t idx, Individual & indi) {
		for (auto& i : m_alternative) {
			if (i.first->variable().index() == indi.variable().index()) {
				if (indi.dominate(*(i.first))) {
					//i.first.release();
					i.first.reset(new Individual(indi));
					i.second = idx;
				}
				return false;
			}
		}
		m_alternative.emplace_back(std::make_pair(std::unique_ptr<Individual>(new Individual(indi)), idx));
		return true;
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::record() {
		//measure::get_measure()->record(global::ms_global.get(), 0, 0);
	}

	template <typename Population1, typename Population2, typename Individual>
	real ADOPT<Population1, Population2, Individual>::cal_standard_deviation(const std::vector<float>& vec1, const std::vector<float>& vec2) {
		if (vec1.size() != vec2.size()) return -1;
		int size = vec1.size();
		real sum = 0;
		for (size_t i = 0; i < size; ++i) {
			sum += pow(vec1[i] - vec2[i], 2);
		}
		return sqrt(sum / (real)size);
	}

	template <typename Population1, typename Population2, typename Individual>
	real ADOPT<Population1, Population2, Individual>::cal_coverage_rate() {
		std::vector<bool> coverage(CAST_RP_EPANET->number_node(), false);
		int count = 0;
		for (size_t i = 0; i < m_alternative.size(); ++i) {
			int idx = m_alternative[i].second;
			for (size_t j = 0; j < m_sub_population[idx].m_first_pop->size(); ++j) {
				int index = (*(m_sub_population[idx].m_first_pop))[j].variable().index() - 1;
				if (!coverage[index]) { coverage[index] = true; ++count; }
			}
		}
		return (real)count / (real)CAST_RP_EPANET->number_node();
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::record_intermediate_result() {
		m_coverage_result.push_back(cal_coverage_rate());
		m_num_pop_result.push_back(m_alternative.size());
		real best = m_alternative[0].first->objective()[0];
		for (size_t i = 1; i < m_alternative.size(); ++i) {
			if (best > m_alternative[i].first->objective()[0])
				best = m_alternative[i].first->objective()[0];
		}
		m_curr_best_obj.push_back(best);
	}

	template <typename Population1, typename Population2, typename Individual>
	void ADOPT<Population1, Population2, Individual>::output() {
		mutex_ADOPT_out.lock();
		std::stringstream path;
		//path << "F:/A_Li_Zhou/Experiment/污水源追踪问题/NET6/40/ADOPT_CCSaDE_NoDistEva_1203.txt";
		path << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/ADOPT/" << m_path_result;

		std::ofstream fout(path.str(), std::ios::app);
		fout << std::endl;

		fout << "runID " << global::ms_global.get()->m_runID << ": " << std::endl;

		for (size_t i = 0; i < m_alternative.size(); ++i) {
			fout << "best solution " << i + 1 << ": " << std::endl;
			fout << "location: " << m_alternative[i].first->variable().location() << std::endl;
			fout << "start time: " << m_alternative[i].first->variable().start_time() << std::endl;
			fout << "duration: " << m_alternative[i].first->variable().duration() << std::endl;
			fout << "multiplier: " << std::endl;
			for (auto &mul_value : m_alternative[i].first->variable().multiplier())
				fout << mul_value << "  ";
			fout << std::endl;
			fout << "obj: " << m_alternative[i].first->objective()[0] << std::endl;
			fout << "standard deviation: " << cal_standard_deviation(m_alternative[i].first->variable().multiplier(), CAST_RP_EPANET->get_optima().variable(0).multiplier()) << std::endl;
			fout << std::endl;
			fout << "concentration: " << std::endl;
			std::vector<std::vector<float>> concen_data(4, std::vector<float>(144));
			CAST_RP_EPANET->get_data(m_alternative[i].first->variable(), concen_data);
			for (size_t j = 0; j < 144; ++j) {
				fout << concen_data[0][j] << " " << concen_data[1][j] << " " << concen_data[2][j] << " " << concen_data[3][j] << " " << (concen_data[0][j] + concen_data[1][j] + concen_data[2][j] + concen_data[3][j]) / 4.0 << std::endl;
			}
		}

		for (size_t i = 0; i<m_coverage_result.size(); ++i)
			fout << i << "   " << m_num_pop_result[i] << "   " << m_coverage_result[i] << std::endl;
		int total_evaluations = CAST_RP_EPANET->total_evaluations();
		fout << "total_evaluations: " << total_evaluations << std::endl;
		fout << "----------------------------------------------------" << std::endl;
		fout.close();

		std::stringstream path2;
		path2 << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/ADOPT/NET2_97/20w/curr_best_obj.txt";
		std::ofstream fout2(path2.str(), std::ios::app);
		for (size_t i = 0; i<m_curr_best_obj.size(); ++i)
			fout2 << i << "   " << m_curr_best_obj[i] << std::endl;
		fout2.close();

		mutex_ADOPT_out.unlock();
	}

	template <typename Population1, typename Population2, typename Individual>
	template <typename Population>
	bool ADOPT<Population1, Population2, Individual>::is_feasible_population(const real tar, const Population & pop) {

		size_t phase = CAST_RP_EPANET->phase();
		size_t interval = CAST_RP_EPANET->interval();
		size_t num = phase*interval;
		real temp = 0;

		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j < CAST_RP_EPANET->num_sensor(); ++j) {
				temp += pow(CAST_RP_EPANET->observation_concentration()[j][i], 2);
			}
		}

		real benchmark = tar * sqrt(temp / (CAST_RP_EPANET->num_sensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < pop.size(); ++i) {
			if (pop[i].objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}

		return count_feasible >= count_infeasible;
	}

}
#endif // OFEC_ADOPT_CCSaDE_H

