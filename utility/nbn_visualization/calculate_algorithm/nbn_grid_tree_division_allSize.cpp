#include "nbn_grid_tree_division_allSize.h"
#include "../../../core/problem/continuous/continuous.h"

namespace ofec {


	void NBN_GridTreeDivision_allSize::vecToIdx(const std::vector<int>& cur,
		int& idx, const std::vector<int>& dim_div) {
		idx = 0;
		for (int idDim(0); idDim < dim_div.size(); ++idDim) {
			idx *= dim_div[idDim];
			idx += cur[idDim];
		}
	}
	void NBN_GridTreeDivision_allSize::solToVec(const SolutionBase& sol,
		std::vector<int>& vec, const std::vector<int>& dim_div,
		const std::vector<double>& range_div,
		const std::vector<double>& range_from
	) {
		auto& cur_sol(dynamic_cast<const Solution<>&>(sol));
		vec.resize(dim_div.size());

		for (int idDim(0); idDim < dim_div.size(); ++idDim) {
			vec[idDim] = (cur_sol.variable()[idDim] - range_from[idDim]) / range_div[idDim];
			if (vec[idDim] >= dim_div[idDim]) vec[idDim] = dim_div[idDim] - 1;
		}


	}
	void NBN_GridTreeDivision_allSize::vecToCenterSol(SolutionBase& sol, const std::vector<int>& vec, const std::vector<int>& dim_div, const std::vector<double>& range_div, const std::vector<double>& range_from)
	{


		auto& cur_sol(dynamic_cast<Solution<>&>(sol));
		for (int idDim(0); idDim < dim_div.size(); ++idDim) {
			cur_sol.variable()[idDim] = range_from[idDim] + (vec[idDim] + 0.5) * range_div[idDim];
		}

	}
	void NBN_GridTreeDivision_allSize::idxToVec(int idx, std::vector<int>& cur, const std::vector<int>& dim_div) {

		for (int idDim(dim_div.size() - 1); idDim >= 0; --idDim) {
			cur[idDim] = idx % dim_div[idDim];
			idx /= dim_div[idDim];
		}
	}



	void NBN_GridTreeDivision_allSize::getSolInNodeId(const SolutionBase& new_sol, const TreeNode& node, int& belong_id) {
		std::vector<int> cur_vec;
		solToVec(new_sol, cur_vec, node.m_dim_div, node.m_range_div, node.m_range_from);
		vecToIdx(cur_vec, belong_id, node.m_dim_div);
	}
	void NBN_GridTreeDivision_allSize::generateSols()
	{

		if (!m_flag_multiThread ) {
			ofec::SolutionBase* ptr_cur_sol;
			int id(0);
			int from_size = m_sols.size();
			assignedSols(m_maxSample - from_size);
			for (int idx(m_sols.size()); idx < m_maxSample; ++idx) {
				ptr_cur_sol = m_environment->problem()->createSolution();
				m_environment->problem()->initializeVariables(ptr_cur_sol->variableBase(), m_random.get());
				m_eval_fun(*ptr_cur_sol, m_environment.get());
				std::shared_ptr<SolutionBase> cur_sol;
				cur_sol.reset(ptr_cur_sol);
				setSol(idx, cur_sol);
				//addRandomSol(*cur_sol, id);
			}
		}
		else {
			int id(0);
			int from_size = m_sols.size();
			int initNum = m_maxSample - from_size;
			std::vector<std::shared_ptr<SolutionBase>> sols(initNum);

			assignedSols(initNum);
			UTILITY::generateRandomSolutionsMultiThreads(sols, m_random.get(), m_environment.get(), m_eval_fun);
			for (int idx(from_size); idx < sols.size(); ++idx) {
				std::shared_ptr<SolutionBase> cur_sol;
				cur_sol=sols[idx];

				setSol(idx, cur_sol);
				//addRandomSol(*cur_sol, id);
			}
			sols.clear();
		}
	}
	void NBN_GridTreeDivision_allSize::getSolId(const SolutionBase& new_sol, std::vector<int>& nodePath) {
		TreeNode& cur_node(*m_root);
		nodePath.clear();
		int belong_idx(0);
		while (true) {
			getSolInNodeId(new_sol, cur_node, belong_idx);
			nodePath.push_back(belong_idx);
			if (cur_node.m_trees[belong_idx] != nullptr) {
				cur_node = *cur_node.m_trees[belong_idx];
			}
			else break;
		}
	}

	void NBN_GridTreeDivision_allSize::assignedSols(int numSols) {
		int originSize(m_sols.size());
		m_sols.resize(m_sols.size() + numSols);
		m_belong.resize(m_sols.size(), -1);
		m_fitness.resize(m_sols.size(), 0);
		m_dis2parent.resize(m_sols.size(), std::numeric_limits<double>::max());
		//m_flagOpt.resize(m_sols.size(), false);
		//m_popIter.resize(m_sols.size(), -1);
		//m_popSolId.resize(m_sols.size(), -1);
		//m_algId.resize(m_sols.size(), -1);

		for (int idx(originSize); idx < m_belong.size(); ++idx) {
			m_belong[idx] = idx;
		}
	}

	void NBN_GridTreeDivision_allSize::updateFitnessThreadTask(int from, int to) {
		for (size_t idx(from); idx < to; ++idx) {
			m_eval_fun(*m_sols[idx], m_environment.get());
			m_fitness[idx] = m_sols[idx]->fitness();
		}
	}


	void NBN_GridTreeDivision_allSize::updateFitness() {
		if (!m_flag_multiThread || !m_flag_evaluate_multitask ) {
			updateFitnessThreadTask(m_solsFitnessUpdatedIdxs, m_sols.size());
		}
		else {
			std::vector<std::thread> thrds;
			int num_task = std::thread::hardware_concurrency();
			int num_samples = m_sols.size() - m_solsFitnessUpdatedIdxs;
			std::vector<int> tasks;
			UTILITY::assignThreads(num_samples, num_task, tasks);
			std::pair<int, int> from_to;
			for (size_t i = 0; i < num_task; ++i) {
				from_to.first = tasks[i] + m_solsFitnessUpdatedIdxs;
				from_to.second = tasks[i + 1] + m_solsFitnessUpdatedIdxs;
				thrds.push_back(std::thread(
					&NBN_GridTreeDivision_allSize::updateFitnessThreadTask, this,
					from_to.first, from_to.second));
			}
			for (auto& thrd : thrds)
				thrd.join();
		}
		m_solsFitnessUpdatedIdxs = m_sols.size();
	}

	int NBN_GridTreeDivision_allSize::updateLocalNetwork(const std::vector<int>& solIdxs)
	{
		std::vector<int> sortedIds(solIdxs);
		std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
			return m_fitness[a] < m_fitness[b];
		});

		double cur_dis(0);
		for (int from_idx(0); from_idx < sortedIds.size(); ++from_idx) {
			auto cur_solId(sortedIds[from_idx]);
			//if (cur_solId == 3235) {
			//	int stop = -1;
			//}
			auto& cur_sol(m_sols[cur_solId]);
			for (int to_idx(from_idx + 1); to_idx < sortedIds.size(); ++to_idx) {
				auto better_solId(sortedIds[to_idx]);

				if (m_fitness[better_solId] >= m_fitness[cur_solId]) {
					auto& better_sol(m_sols[better_solId]);
					cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), better_sol->variableBase());
					if (cur_dis < m_dis2parent[cur_solId] || (cur_dis == m_dis2parent[cur_solId] && m_random->uniform.next() < 0.5)) {
						m_dis2parent[cur_solId] = cur_dis;
						m_belong[cur_solId] = better_solId;
					}
				}
			}

		}

		return sortedIds.back();
	}


	void NBN_GridTreeDivision_allSize::generateNetwork() {
		
		m_dim = m_environment->problem()->numberVariables();
		std::vector<int> solIds(m_sols.size());
		for (int idx(0); idx < m_sols.size(); ++idx) {
			solIds[idx] = idx;
		}
		if (judgeDivisionFlag(solIds.size())) {
			m_root.reset(new TreeNode);
			m_root->m_depth = 0;
			const auto& boundary = CAST_CONOP(m_environment->problem())->boundary();
			updateNetwork(*m_root, solIds,boundary);
		}
		else {
			updateLocalNetwork(solIds);
		}
	}



	void NBN_GridTreeDivision_allSize::calculateNetworkAccurate(TreeNode& curTreeNode) {
		std::vector<int> sorted_idx(curTreeNode.m_division_nodes.size());
		for (int idx(0); idx < sorted_idx.size(); ++idx) {
			sorted_idx[idx] = idx;
		}
		auto& nbn_nodes(curTreeNode.m_division_nodes);
		std::sort(sorted_idx.begin(), sorted_idx.end(), [&](
			int a, int  b
			) {
			return nbn_nodes[a].m_representative_sol->fitness() <
				nbn_nodes[b].m_representative_sol->fitness();
		});

		curTreeNode.m_cur_visited2 = 0;
		for (auto& curNode : curTreeNode.m_division_nodes) {
			curNode.m_better_range_nodeId.clear();
			curNode.m_better_range_nodeId.push_back(curNode.m_node_id);
		}
		for (int sidx(0); sidx + 1 < sorted_idx.size(); ++sidx) {
			//std::cout << "curId\t" << sidx << std::endl;
			auto sortedId = sorted_idx[sidx];
			auto& cur_node(nbn_nodes[sortedId]);

			updateNeighborAndParent(curTreeNode, cur_node);

		}
		curTreeNode.m_best_divNodeId = sorted_idx.back();
		curTreeNode.m_best_solId = nbn_nodes[sorted_idx.back()].m_representative_solId;
	}

	//void NBN_GridTreeDivision_allSize::calSolsNetworkThreadTask(int from, int to, std::vector<int>& nodeId, const std::vector<int>& solIdxs)
	//{
	//}

	void NBN_GridTreeDivision_allSize::divideSolsNetwork(TreeNode& curNode, const std::vector<int>& solIdxs)
	{

		std::vector<int> belongNodeIds(solIdxs.size(), -1);
		int belongNodeId = 0;
		for (int idx(0); idx < solIdxs.size(); ++idx) {
			auto& solId = solIdxs[idx];
			getSolInNodeId(*m_sols[solId], curNode, belongNodeId);
			//if (belongNodeId < 0) {
			//	getSolInNodeId(*m_sols[solId], curNode, belongNodeId);
			//}
			belongNodeIds[idx] = belongNodeId;
		}

		for (int idx(0); idx < solIdxs.size(); ++idx) {
			auto& solId = solIdxs[idx];
			auto& nodeId = belongNodeIds[idx];
			curNode.m_division_nodes[nodeId].m_sample_solIds.push_back(solId);
		}

		for (auto& it : curNode.m_division_nodes) {

			if (it.m_sample_solIds.empty()) {
				it.m_representative_sol.reset(m_environment->problem()->createSolution());
				//SolContinousType& cursol = dynamic_cast<SolContinousType&>(*it.m_representative_sol);
				vecToCenterSol(*it.m_representative_sol, it.m_vec, curNode.m_dim_div, curNode.m_range_div, curNode.m_range_from);
				it.m_representative_sol->setFitness(m_worstFit);
			}
		}

	}

	void NBN_GridTreeDivision_allSize::updateNetwork(
		TreeNode& curNode, 
		const std::vector<int>& solIdxs,
		const std::vector<std::pair<double, double>>& boundary) {

		curNode.initialize(boundary, solIdxs.size(),m_user_div);

		divideSolsNetwork(curNode, solIdxs);


		std::vector<std::pair<double, double>> curBoundary;

		for (auto& it : curNode.m_division_nodes) {

			if (!it.m_sample_solIds.empty()) {
				if (judgeDivisionFlag(it.m_sample_solIds.size())) {
					curNode.getSubSpace(it.m_node_id, curBoundary);
					curNode.m_trees[it.m_node_id].reset(new TreeNode);
					curNode.m_trees[it.m_node_id]->m_depth = curNode.m_depth + 1;
					//if (it.m_node_id == 2600) {
					//	int stop = -1;
					//}
					updateNetwork(
						*curNode.m_trees[it.m_node_id],
						it.m_sample_solIds,
						curBoundary);

					it.m_representative_solId = curNode.m_trees[it.m_node_id]->m_best_solId;
					it.m_representative_sol = m_sols[it.m_representative_solId];
				
				}
				else {
					it.m_representative_solId = updateLocalNetwork(it.m_sample_solIds);
					it.m_representative_sol = m_sols[it.m_representative_solId];
				}
			}
		}
		calculateNetworkAccurate(curNode);
	}



	void NBN_GridTreeDivision_allSize::TreeNode::getSubSpace(int nodeId, std::vector<std::pair<double, double>>& boundary) {
		boundary.resize(m_boundary.size());
		std::vector<int> vec(m_dim_div);
		idxToVec(nodeId, vec, m_dim_div);
		for (int idDim(0); idDim < boundary.size(); ++idDim) {
			boundary[idDim].first = m_boundary[idDim].first + vec[idDim] * m_range_div[idDim];
			boundary[idDim].second = boundary[idDim].first + m_range_div[idDim];
		}
	}



	void NBN_GridTreeDivision_allSize::TreeNode::resizeNumberDivision(int numDiv) {
		m_trees.resize(numDiv, nullptr);
		m_neighbors.clear();
		//m_division_nodes.clear();
		m_division_nodes.resize(numDiv);
		for (int idx(0); idx < numDiv; ++idx) {
			m_division_nodes[idx].initialize(idx);
		}
	}

	void NBN_GridTreeDivision_allSize::TreeNode::initialize(
		const std::vector<std::pair<double, double>>& boundary, int sampleSize,
		const std::vector<int>& user_div
	) {
		m_boundary = boundary;
		int dim(m_boundary.size());
		int numSample_int = sampleSize;
		m_dim_div.resize(dim);
		int dim_div = std::max(2.0, exp(log(double(numSample_int)) / double(dim)));
		if (user_div.size() > m_depth) {
			dim_div = user_div[m_depth];
		}
		numSample_int = 1;
		for (auto& it : m_dim_div) {
			it = dim_div;
			numSample_int *= it;
		}
		m_numberDivision = numSample_int;

		m_range_div.resize(dim);
		m_range_from.resize(dim);
		for (int idDim(0); idDim < dim; ++idDim) {
			auto& range_var = m_boundary[idDim];
			m_range_div[idDim] = (range_var.second - range_var.first) / double(m_dim_div[idDim]);
			m_range_from[idDim] = range_var.first;
		}
		resizeNumberDivision(m_numberDivision);

		std::vector<int> curVec(dim, 0);
		int curIdx(0);
		// set solutions
		for (size_t idx(0); idx < m_numberDivision; ++idx) {
			idxToVec(idx, curVec, m_dim_div);
			vecToIdx(curVec, curIdx, m_dim_div);
			m_division_nodes[idx].m_vec = curVec;
		}
		// generate neighbor infos
		{
			std::vector<int> neiVec;
			int neiIdx(0);
			for (int idx(0); idx < m_numberDivision; ++idx) {
				auto& cur_node = m_division_nodes[idx];
				idxToVec(idx, curVec, m_dim_div);
				vecToIdx(curVec, curIdx, m_dim_div);
				neiVec = curVec;
				for (int idDim(0); idDim < dim; ++idDim) {
					--neiVec[idDim];
					if (neiVec[idDim] >= 0) {
						vecToIdx(neiVec, neiIdx, m_dim_div);
						auto& nei_node = m_division_nodes[neiIdx];
						cur_node.m_neighbor_nodeIds.push_back(neiIdx);
						nei_node.m_neighbor_nodeIds.push_back(curIdx);
						//			m_neighbors.push_back({ idx, neiIdx });
					}
					++neiVec[idDim];
				}
			}

		}

	}
}