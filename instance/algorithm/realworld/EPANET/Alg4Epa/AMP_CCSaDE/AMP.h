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
// updated Jun 10, 2019 by Li Zhou

#ifndef OFEC_AMP_H
#define OFEC_AMP_H

#include "../../../../../../core/algorithm/algorithm.h"
#include "../DynSaMultiPop.h"
#include "../CC_framework.h"

namespace OFEC {

	//extern struct Cluster;
	template <typename Population1, typename Population2, typename Individual>
	class AMP : public algorithm
	{
		std::mutex mutex_AMP_out;
		//using fill_type = CC_framework<Population1, Population2, Individual>::fill_type;

	public:
		AMP(param_map & v);
		void run_();
		void initialize();
	protected:
		evaluation_tag evolve();

		/*k-means----start*/
		std::vector<Cluster> k_means(std::vector<int> & trainX, size_t k);
		double cal_dis(const std::vector<real> & point1, const std::vector<real> & point2);
		double cal_var(const std::vector<Cluster> & clusters);
		int clustering(const std::vector<Cluster>& clusters, const int id_trainX);
		void getMeans(std::vector<Cluster> & clusters, const std::vector<int> & trainX);
		/*k-means----end*/

		int competition();
		void compute_node_distance(std::vector<real> & vect);
		void add_new_pop(size_t num);
		void output();
		bool is_stable_by_popNum();

		bool is_stable_by_obj();
		void update_distance(size_t idx);
		bool is_stable();

		real cal_coverage_rate();
		void record_intermediate_result();
		real cal_standard_deviation(const std::vector<float>& vec1, const std::vector<float>& vec2);

		void record() {}
	private:
		DynSaMultiPop<CC_framework<Population1, Population2, Individual>> m_subpop;
		int m_num_add;
		const std::string m_path_result;
		size_t m_data_update_eval;
		int m_iter = 0;
		bool m_is_first_add = true;
		std::vector<int> m_num_be_removed;
		size_t m_num_curr_pop;

		size_t m_k;
		size_t m_standard_stable;
		real m_max_coverge_rate;
		std::vector<int> m_train_data;

		std::vector<real> m_coverage_result;
		std::vector<size_t> m_num_pop_result;
		std::vector<real> m_curr_best_obj;

		size_t m_max_eval;
		size_t m_initial_phase;
		size_t m_final_phase;

		size_t m_increment;
	};

	//std::mutex mutex_AMP_out;
	template <typename Population1, typename Population2, typename Individual>
	AMP<Population1, Population2, Individual>::AMP(param_map & v) : algorithm(v.at("algName")), \
		m_subpop((size_t)v.at("numSubPop"), (size_t)v.at("subPopSize")), m_num_add((size_t)v.at("numSubPop") / 2), m_k((size_t)v.at("numSubPop")), \
		m_num_curr_pop((size_t)v.at("numSubPop")), m_path_result(v.at("dataFileResult"))
	{
		m_standard_stable = v.at("beta");
		m_increment = v.at("alpha");
		m_max_eval = v.at("maxEvals");
		m_initial_phase = CAST_RP_EPANET->phase();
		m_final_phase = CAST_RP_EPANET->m_total_phase;
		m_data_update_eval = m_max_eval / (m_final_phase - m_initial_phase + 1);
		//m_max_coverge_rate = v.at("beta");

		size_t node_size = CAST_RP_EPANET->number_node();
		for (size_t i = 1; i <= node_size; ++i) {
			m_train_data.push_back(i);
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::initialize() {

		CAST_RP_EPANET->m_clusters = k_means(m_train_data, m_k);

		for (size_t i = 0; i < m_subpop.size(); ++i) {  // initialize subpopulation 

			CAST_RP_EPANET->set_init_type(init_type::k_means);
			CAST_RP_EPANET->m_pop_identifier = i;
			m_subpop[i].m_first_pop->initialize();

			m_subpop[i].m_second_pop->initialize();

			m_subpop[i].fill_each(CC_framework<Population1, Population2, Individual>::fill_type::random);

			m_subpop[i].update_best();

			//competition();
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::run_() {
		evaluation_tag tag = evaluation_tag::Normal;
		size_t num_pop_last_iter = m_num_curr_pop;
		while (tag != evaluation_tag::Terminate) {

			record_intermediate_result();

			tag = evolve();

			int num_remove = competition();
			m_num_be_removed.push_back(num_remove);
			m_num_curr_pop -= num_remove;

			if (is_stable()) {

				m_num_be_removed.clear();

				CAST_RP_EPANET->m_clusters.clear();

				if (m_is_first_add) {
					CAST_RP_EPANET->m_clusters = k_means(m_train_data, m_num_add);

					add_new_pop(m_num_add);

					m_is_first_add = false;
				}
				else {
					int differ = num_pop_last_iter - m_num_curr_pop;
					if (differ < m_num_add)
						++m_num_add;
					else if (differ > m_num_add)
						--m_num_add;
					CAST_RP_EPANET->m_clusters = k_means(m_train_data, m_num_add);
					add_new_pop(m_num_add);
				}
				num_pop_last_iter = m_num_curr_pop;
			}

			size_t curr_eval = global::ms_global->m_problem->evaluations();

			//std::cout << "runID: " << global::ms_global.get()->m_runID << "  curr_eval: " << curr_eval << "  iteration: " << m_iter << "  pop_num: " << m_num_pop_result[m_iter-1] << "  coverage: " << m_coverage_result[m_iter-1] <<std::endl;
			//std::cout << "m_standard_stable: " << m_standard_stable << std::endl;

			if (curr_eval >= (CAST_RP_EPANET->phase() - m_initial_phase + 1) * m_data_update_eval) {
				CAST_RP_EPANET->phase_next();
				if (CAST_RP_EPANET->is_time_out()) break;
				if (CAST_RP_EPANET->phase() % m_increment == 0)
					m_standard_stable += 1;
			}
		}

		output();

	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::record_intermediate_result() {
		m_coverage_result.push_back(cal_coverage_rate());
		m_num_pop_result.push_back(m_subpop.size());
		real best = m_subpop[0].m_best->objective()[0];
		for (size_t i = 1; i < m_subpop.size(); ++i) {
			if (best > m_subpop[i].m_best->objective()[0])
				best = m_subpop[i].m_best->objective()[0];
		}
		m_curr_best_obj.push_back(best);
	}

	template <typename Population1, typename Population2, typename Individual>
	real AMP<Population1, Population2, Individual>::cal_coverage_rate() {
		std::vector<bool> coverage(CAST_RP_EPANET->number_node(), false);
		int count = 0;
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			for (size_t j = 0; j < m_subpop[i].m_first_pop->size(); ++j) {
				int index = (*(m_subpop[i].m_first_pop))[j].variable().index() - 1;
				if (!coverage[index]) { coverage[index] = true; ++count; }
			}
		}
		return (real)count / (real)CAST_RP_EPANET->number_node();
	}

	template <typename Population1, typename Population2, typename Individual>
	real AMP<Population1, Population2, Individual>::cal_standard_deviation(const std::vector<float>& vec1, const std::vector<float>& vec2) {
		if (vec1.size() != vec2.size()) return -1;
		int size = vec1.size();
		real sum = 0;
		for (size_t i = 0; i < size; ++i) {
			sum += pow(vec1[i] - vec2[i], 2);
		}
		return sqrt(sum / (real)size);
	}

	template <typename Population1, typename Population2, typename Individual>
	evaluation_tag AMP<Population1, Population2, Individual>::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;

		for (size_t i = 0; i < m_subpop.size(); ++i) {

			m_subpop[i].m_first_pop->update_probability();
			//m_subpop[i].m_second_pop->update_probability();
			m_subpop[i].m_second_pop->update_CR();
			m_subpop[i].m_second_pop->update_F();
			m_subpop[i].m_second_pop->update_pro_strategy();

			m_subpop[i].m_first_pop->mutate();
			m_subpop[i].m_second_pop->mutate();

			m_subpop[i].m_second_pop->recombine();

			/*if (m_subpop[i].is_stable_obj()) {
			update_distance(i);
			m_subpop[i].m_first_pop->select(true);
			m_subpop[i].m_second_pop->select(true);

			}
			else {
			m_subpop[i].m_first_pop->select(false);
			m_subpop[i].m_second_pop->select(false);
			}*/
			m_subpop[i].m_first_pop->select(false);
			m_subpop[i].m_second_pop->select(false);

			m_subpop[i].fill_each(CC_framework<Population1, Population2, Individual>::fill_type::best);
			//m_subpop[i].record_history_mass();
		}
		++m_iter;
		return tag;
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::update_distance(size_t idx) {
		real min_distance;

		for (size_t j = 0; j < m_subpop[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_subpop[idx].m_first_pop))[j].variable().index(), m_subpop[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_first_pop))[j].m_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_subpop[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_subpop[idx].m_second_pop))[j].variable().index(), m_subpop[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_second_pop))[j].m_distance_fitness = min_distance;
		}

		for (size_t j = 0; j < m_subpop[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_subpop[idx].m_first_pop))[j].trial().variable().index(), m_subpop[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_first_pop))[j].m_pu_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_subpop[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				real dis = CAST_RP_EPANET->calculate_distance((*(m_subpop[idx].m_second_pop))[j].trial().variable().index(), m_subpop[k].m_best->variable().index());

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_second_pop))[j].m_pu_distance_fitness = min_distance;
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	bool AMP<Population1, Population2, Individual>::is_stable_by_popNum() {
		if (m_num_be_removed.size() < m_standard_stable) return false;
		int i = m_num_be_removed.size() - 1;
		int count = 0;
		while (m_num_be_removed[i] == 0 && m_num_be_removed[i - 1] == 0) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= m_standard_stable - 1;
	}

	template <typename Population1, typename Population2, typename Individual>
	bool AMP<Population1, Population2, Individual>::is_stable_by_obj() {
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			if (!m_subpop[i].is_stable_obj()) {
				return false;
			}
		}
		return true;
	}

	template <typename Population1, typename Population2, typename Individual>
	bool AMP<Population1, Population2, Individual>::is_stable() {

		/*for (size_t i = 0; i < m_subpop.size(); ++i) {
		if (!(m_subpop[i].is_stable_mass()))
		return false;
		}
		return true;*/

		if (m_coverage_result.size() < m_standard_stable) return false;
		int i = m_coverage_result.size() - 1;
		if (m_coverage_result[i] == 1) return false;
		int count = 0;
		while (m_coverage_result[i] == m_coverage_result[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= m_standard_stable - 1;
	}

	template <typename Population1, typename Population2, typename Individual>
	double AMP<Population1, Population2, Individual>::cal_dis(const std::vector<real> & point1, const std::vector<real> & point2) {
		size_t da = point1.size();
		size_t db = point2.size();
		if (da != db) throw myexcept("AMP::cal_dis(): Dimensions of two vectors must be same!!");
		double val = 0.0;
		for (size_t i = 0; i < da; i++)
		{
			val += pow((point1[i] - point2[i]), 2);
		}
		return pow(val, 0.5);
	}

	template <typename Population1, typename Population2, typename Individual>
	double AMP<Population1, Population2, Individual>::cal_var(const std::vector<Cluster> & clusters) {
		double var = 0;
		for (int i = 0; i < clusters.size(); i++)
		{
			Cluster t = clusters[i];
			for (int j = 0; j< t.samples.size(); j++)
			{
				var += cal_dis(CAST_RP_EPANET->get_coordinates(t.samples[j] + 1), t.centroid);
			}
		}
		return var;
	}

	template <typename Population1, typename Population2, typename Individual>
	int AMP<Population1, Population2, Individual>::clustering(const std::vector<Cluster>& clusters, const int id_trainX) {
		std::vector<real> sample = CAST_RP_EPANET->get_coordinates(id_trainX);
		double dist = cal_dis(clusters[0].centroid, sample);
		double tmp;
		int label = 0;//标示属于哪一个簇
		for (int i = 1; i < clusters.size(); i++) {
			tmp = cal_dis(clusters[i].centroid, sample);
			if (tmp<dist) { dist = tmp; label = i; }
		}
		return label;
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::getMeans(std::vector<Cluster> & clusters, const std::vector<int> & trainX) {
		for (size_t i = 0; i < clusters.size(); i++)
		{
			size_t dim = 2;
			std::vector<double> val(dim, 0.0);
			for (size_t j = 0; j < clusters[i].samples.size(); j++)
			{
				size_t sample = clusters[i].samples[j];
				for (size_t d = 0; d < dim; d++)
				{
					val[d] += CAST_RP_EPANET->get_coordinates(trainX[sample])[d];
					if (j == clusters[i].samples.size() - 1)
						clusters[i].centroid[d] = val[d] / clusters[i].samples.size();
				}
			}
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	std::vector<Cluster> AMP<Population1, Population2, Individual>::k_means(std::vector<int> & trainX, size_t k) {
		const size_t size = trainX.size();

		/*初始化聚类中心*/
		std::vector<Cluster> clusters(k);
		for (size_t i = 0; i < k; i++)
		{
			int c = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, size);
			clusters[i].centroid = CAST_RP_EPANET->get_coordinates(trainX[c]);
		}

		//根据默认的质心给簇赋值
		int lable = 0;
		for (size_t i = 0; i != trainX.size(); ++i) {
			lable = clustering(clusters, trainX[i]);
			clusters[lable].samples.push_back(i);
		}
		double oldVar = -1;
		double newVar = cal_var(clusters);
		/*多次迭代直至收敛*/
		int count = 0;
		while (fabs(newVar - oldVar) >= 1)
		{
			++count;
			//更新每个簇的中心点
			getMeans(clusters, trainX);

			oldVar = newVar;
			newVar = cal_var(clusters); //计算新的准则函数值
			for (size_t i = 0; i < k; i++) //清空每个簇
			{
				clusters[i].samples.clear();
			}
			//根据新的质心获得新的簇
			for (size_t i = 0; i != trainX.size(); ++i) {
				lable = clustering(clusters, trainX[i]);
				clusters[lable].samples.push_back(i);
			}
		}
		return clusters;
	}

	template <typename Population1, typename Population2, typename Individual>
	int AMP<Population1, Population2, Individual>::competition() {
		std::vector<size_t> subpop_indx;
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			bool flag = false;
			for (size_t j = 0; j < subpop_indx.size(); ++j) {
				/*if (m_subpop.is_high_coverage_rate(subpop_indx[j], i, m_max_coverge_rate)) {*/
				if (m_subpop[subpop_indx[j]].m_best->variable().index() == m_subpop[i].m_best->variable().index()
					/*&& m_subpop[subpop_indx[j]].is_stable_obj() && m_subpop[i].is_stable_obj()*/) {
					m_subpop.population_competition(subpop_indx[j], i);
					if (m_subpop[i].m_best->objective()[0] < m_subpop[subpop_indx[j]].m_best->objective()[0])
						*(m_subpop[subpop_indx[j]].m_best) = *(m_subpop[i].m_best);

					flag = true;
					break;
				}
			}
			if (!flag)
				subpop_indx.push_back(i);
		}
		int differ1 = m_subpop.size();
		int differ2 = subpop_indx.size();
		int differ = differ1 - differ2;
		int return_value = differ;
		if (differ > 0) {
			DynSaMultiPop<CC_framework<GL_population, SaDE_population, epanet_individual>> temp(subpop_indx.size(), m_subpop[0].m_first_pop->size());
			for (size_t i = 0; i < subpop_indx.size(); ++i) {
				temp[i] = m_subpop[subpop_indx[i]];
			}
			for (size_t i = 0; i < subpop_indx.size(); ++i) {
				m_subpop[i] = temp[i];
				//m_subpop[i].clear_history();
			}
		}

		while (differ) {
			m_subpop.pop_back();
			--differ;
		}
		return return_value;
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::add_new_pop(size_t num) {
		compute_node_distance(CAST_RP_EPANET->m_distance_node);
		//m_subpop.add_new_population(num / 2, init_type::random);
		//m_subpop.add_new_population(num - (num / 2), init_type::be_visited);
		m_subpop.add_new_population(num, init_type::be_visited);
		m_num_curr_pop += num;
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::compute_node_distance(std::vector<real> & vect) {
		size_t node_size = CAST_RP_EPANET->number_node();
		vect.resize(node_size);
		for (size_t i = 0; i < node_size; ++i) {
			real curr_best;
			for (size_t j = 0; j < m_subpop.size(); ++j) {
				real value = CAST_RP_EPANET->calculate_distance(i + 1, m_subpop[j].m_best->variable().index());
				if (j == 0) curr_best = value;
				else if (curr_best > value) curr_best = value;
			}
			vect[i] = curr_best;
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	void AMP<Population1, Population2, Individual>::output() {
		mutex_AMP_out.lock();
		std::stringstream path;
		path << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/AMP/" << m_path_result;
		std::ofstream fout(path.str(), std::ios::app);
		fout << std::endl;
		fout << "runID " << global::ms_global.get()->m_runID << ": " << std::endl;

		for (size_t i = 0; i < m_subpop.size(); ++i) {
			fout << "best solution " << i + 1 << ": " << std::endl;
			fout << "location: " << m_subpop[i].m_best->variable().location() << std::endl;
			fout << "start time: " << m_subpop[i].m_best->variable().start_time() << std::endl;
			fout << "duration: " << m_subpop[i].m_best->variable().duration() << std::endl;
			fout << "multiplier: " << std::endl;
			for (auto &mul_value : m_subpop[i].m_best->variable().multiplier())
				fout << mul_value << "  ";
			fout << std::endl;
			fout << "obj: " << m_subpop[i].m_best->objective()[0] << std::endl;
			fout << "standard deviation: " << cal_standard_deviation(m_subpop[i].m_best->variable().multiplier(), CAST_RP_EPANET->get_optima().variable(0).multiplier()) << std::endl;
			fout << std::endl;
			fout << "concentration: " << std::endl;
			std::vector<std::vector<float>> concen_data(4, std::vector<float>(144));
			CAST_RP_EPANET->get_data(m_subpop[i].m_best->variable(), concen_data);
			for (size_t j = 0; j < 144; ++j) {
				fout << concen_data[0][j] << " " << concen_data[1][j] << " " << concen_data[2][j] << " " << concen_data[3][j] << " " << (concen_data[0][j] + concen_data[1][j] + concen_data[2][j] + concen_data[3][j]) / 4.0 << std::endl;
			}
		}

		for (size_t i = 0; i<m_coverage_result.size(); ++i)
			fout << i << "   " << m_num_pop_result[i] << "   " << m_coverage_result[i] << std::endl;

		fout << "----------------------------------------------------" << std::endl;

		fout.close();

		std::stringstream path2;
		path2 << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/AMP/NET2_97/20w/curr_best_obj.txt";
		std::ofstream fout2(path2.str(), std::ios::app);
		for (size_t i = 0; i<m_curr_best_obj.size(); ++i)
			fout2 << i << "   " << m_curr_best_obj[i] << std::endl;
		fout2.close();

		mutex_AMP_out.unlock();
	}

}

#endif