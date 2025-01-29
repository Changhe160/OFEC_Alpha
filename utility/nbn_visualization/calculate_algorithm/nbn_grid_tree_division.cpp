#include "nbn_grid_tree_division.h"
#include "nbn_grid_tree_division.h"
#include "../../function/custom_function.h"
#include "../../../core/problem/continuous/continuous.h"
#include <queue>
namespace ofec {

	void NBN_GridTreeDivision::vecToIdx(const std::vector<int>& cur,
		int& idx, const std::vector<int>& dim_div) {
		idx = 0;
		for (int idDim(0); idDim < dim_div.size(); ++idDim) {
			idx *= dim_div[idDim];
			idx += cur[idDim];
		}
	}
	void NBN_GridTreeDivision::solToVec(const SolutionBase& sol,
		std::vector<int>& vec, const std::vector<int>& dim_div,
		const std::vector<double>& range_div,
		const std::vector<double>& range_from
	) {
		auto& cur_sol(dynamic_cast<const Solution<>&>(sol));
		vec.resize(dim_div.size());

		for (int idDim(0); idDim < dim_div.size(); ++idDim) {
			vec[idDim] = (cur_sol.variable()[idDim] - range_from[idDim]) / range_div[idDim];
			if (vec[idDim] >= dim_div[idDim]) vec[idDim] = dim_div[idDim]-1;
		}

		
	}
	void NBN_GridTreeDivision::idxToVec(int idx, std::vector<int>& cur, const std::vector<int>& dim_div) {

		for (int idDim(dim_div.size() - 1); idDim >= 0; --idDim) {
			cur[idDim] = idx % dim_div[idDim];
			idx /= dim_div[idDim];
		}
	}


	const SolutionBase* NBN_GridTreeDivision::getSol(int solId) const {
		if (solId<0 || solId>m_sols.size()) {
			throw Exception("nbn error at get solution: wrong id");
			return nullptr;
		}
		else {
			return m_sols[solId].get();
		}
	}

	void NBN_GridTreeDivision::TreeNode::resizeNumberDivision(int numDiv) {
		m_trees.resize(numDiv, nullptr);
		m_neighbors.clear();
	    //m_division_nodes.clear();
		m_division_nodes.resize(numDiv);
		for (int idx(0); idx < numDiv; ++idx) {
			m_division_nodes[idx].initialize(idx);
		}
	}
	void NBN_GridTreeDivision::TreeNode::initialize(
		const std::vector<std::pair<double, double>>& boundary, int sampleSize) {
		m_boundary = boundary;
		int dim(m_boundary.size());
		int numSample_int = sampleSize;
		m_dim_div.resize(dim);
		int dim_div = std::max(2.0,exp(log(double(numSample_int)) / double(dim)));
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

	void NBN_GridTreeDivision::TreeNode::getSubSpace(int nodeId, std::vector<std::pair<double, double>>& boundary) {
		boundary.resize(m_boundary.size());
		std::vector<int> vec(m_dim_div);
		idxToVec(nodeId, vec, m_dim_div);
		for (int idDim(0); idDim < boundary.size(); ++idDim) {
			boundary[idDim].first = m_boundary[idDim].first + vec[idDim] * m_range_div[idDim];
			boundary[idDim].second = boundary[idDim].first + m_range_div[idDim];
		}
	}


	void NBN_GridTreeDivision::TreeNode::mergeParent(Node* curNode) {
		if (curNode->m_parent_nodeId == curNode->m_node_id) return;
		std::vector<Node*> nodes;
		Node* curVisNode = curNode;
		while (curVisNode->m_parent_nodeId != curVisNode->m_node_id) {
			nodes.push_back(curVisNode);
			curVisNode = &m_division_nodes[curVisNode->m_parent_nodeId];
		}
		for (auto& it : nodes) {
			it->m_parent_nodeId = curVisNode->m_node_id;
		}
	}

	void NBN_GridTreeDivision::TreeNode::findParent(Node* curNode, Node*& parent){
		if (curNode->m_parent_nodeId == curNode->m_node_id) {
			parent = curNode;
			return;
	    }
		//std::vector<Node*> nodes;
		Node* curVisNode = curNode;
		while (curVisNode->m_parent_nodeId != curVisNode->m_node_id) {
			//nodes.push_back(curVisNode);
			curVisNode = &m_division_nodes[curVisNode->m_parent_nodeId];
		}
		parent = curVisNode;
	}
	// grid random sample solution
	void NBN_GridTreeDivision::generateSolution(TreeNode& curNode) {
		auto pro = m_environment->problem();
		int solIdx(m_sols.size());
		assignedSols(curNode.m_numberDivision);
		std::vector<double> startPos(m_dim, 0);

		for (int idDim(0); idDim < m_dim; ++idDim) {
			startPos[idDim] = curNode.m_boundary[idDim].first ;
		}
		for (int nodeId(0); nodeId < curNode.m_numberDivision; ++nodeId) {
			auto& cur_sol(m_sols[solIdx]);
			cur_sol.reset(new Solution<>(
				pro->numberObjectives(),
				pro->numberConstraints(),
				pro->numberVariables()));
		//	cur_sol->setId(solIdx);
			auto& curConSol(dynamic_cast<Solution<>&>(*cur_sol));
			const auto& curVec(curNode.m_division_nodes[nodeId].m_vec);

			for (int idDim(0); idDim < m_dim; ++idDim) {
				curConSol.variable()[idDim] = startPos[idDim] + curVec[idDim] * curNode.m_range_div[idDim] + curNode.m_range_div[idDim] * m_random->uniform.next();
			}

			curNode.m_division_nodes[nodeId].m_sample_solIds.push_back(solIdx);
			++solIdx;
		}
	}

	// grid sample
	//void NBN_GridTreeDivision::generateSolution(TreeNode& curNode) {
	//	int solIdx(m_sols.size());
	//	assignedSols(curNode.m_numberDivision);
	//	std::vector<double> startPos(m_dim,0);

	//	for (int idDim(0); idDim < m_dim; ++idDim) {
	//		startPos[idDim] = curNode.m_boundary[idDim].first + curNode.m_range_div[idDim]/2.0;
	//	}
	//	for (int nodeId(0); nodeId < curNode.m_numberDivision; ++nodeId) {
	//		auto& cur_sol(m_sols[solIdx]);
	//		cur_sol.reset(new Solution<>(
	//			m_environment->problem()->numberObjectives(),
	//			m_environment->problem()->numberConstraints(),
	//			m_environment->problem()->numberVariables()));
	//		cur_sol->setId(solIdx);
	//		auto& curConSol(dynamic_cast<Solution<>&>(*cur_sol));
	//		const auto& curVec(curNode.m_division_nodes[nodeId].m_vec);

	//		for (int idDim(0); idDim < m_dim; ++idDim) {
	//			curConSol.variable()[idDim] = startPos[idDim] + curVec[idDim] * curNode.m_range_div[idDim];
	//		}

	//		curNode.m_division_nodes[nodeId].m_sample_solIds.push_back(solIdx);
	//		++solIdx;
	//	}
	//}
	void NBN_GridTreeDivision::divideSubSpace(std::shared_ptr<TreeNode>& curTreeNode, int idx) {

		std::vector<std::pair<double, double>> div_boundary;
		auto& sonNode(curTreeNode->m_division_nodes[idx]);
		curTreeNode->getSubSpace(idx, div_boundary);
		curTreeNode->m_trees[idx].reset(new TreeNode);
		auto& sonTreeNode(curTreeNode->m_trees[idx]);
		divideSpace(sonTreeNode, div_boundary, m_numNewSample);
		insertNewSolsIntoTreeNode(*sonTreeNode, sonNode.m_sample_solIds);
		sonNode.m_sample_solIds.clear();
		sonTreeNode->m_parent = curTreeNode;
		sonTreeNode->m_son_nodeId = idx;

		
	//	sonTreeNode->m_best_solId 
	}


	//void NBN_GridTreeDivision::updateDivision() {
	//	const auto& boundary = CAST_CONOP(m_environment->problem().get())->boundary();
	//	divideSpace(m_root, boundary, m_maxSample);
	//}
	//void NBN_GridTreeDivision::updateDivision(std::shared_ptr<TreeNode>& curTreeNode, const std::vector<std::pair<double, double>>& boundary, int sampleSize) {



	//	updateFitness();
	//	if (m_flag_addOpt) {
	//		addOpt();
	//	}
	//	//std::cout << "cur sol size\t" << m_sols.size() << std::endl;
	//	for (int idx(0); idx < curTreeNode->m_division_nodes.size(); ++idx) {
	//		udpateSubspace(curTreeNode, idx);
	//	}

	//	//// for test
	//	//{
	//	//	for (int idx(0); idx < m_belong.size(); ++idx) {
	//	//		if (m_fitness[idx] > m_fitness[m_belong[idx]]) {
	//	//			int stop = -1;
	//	//			std::cout << "error" << std::endl;
	//	//		}
	//	//	}
	//	//}

	//	//calculateNetwork(*curTreeNode);
	//	calculateNetworkAccurate(*curTreeNode);

	//	//for(int idx(0);idx<curNode.m_fi)

	//	//curNode.initialize()
	//}

	void NBN_GridTreeDivision::divideSpace(std::shared_ptr<TreeNode>& curTreeNode, const std::vector<std::pair<double, double>>& boundary, int sampleSize)
	{
		

		curTreeNode->initialize(boundary, sampleSize);
		generateSolution(*curTreeNode);
		int leftSize = m_maxSample - m_sols.size();
		//std::cout << "maxSize\t"<<m_maxSample<<"\tleftSize\t" << leftSize << std::endl;
		addRandom(leftSize);

		for (auto& it : m_added_sols) {
			int belong_id(0);
			addSol(*it, belong_id);
		}
		m_added_sols.clear();


		updateFitness();
		if (m_flag_addOpt) {
			addOpt();
		}
		//std::cout << "cur sol size\t" << m_sols.size() << std::endl;
		for (int idx(0); idx < curTreeNode->m_division_nodes.size(); ++idx) {
			udpateSubspace(curTreeNode, idx);
		}

		//// for test
		//{
		//	for (int idx(0); idx < m_belong.size(); ++idx) {
		//		if (m_fitness[idx] > m_fitness[m_belong[idx]]) {
		//			int stop = -1;
		//			std::cout << "error" << std::endl;
		//		}
		//	}
		//}

		//calculateNetwork(*curTreeNode);
		calculateNetworkAccurate(*curTreeNode);

		//for(int idx(0);idx<curNode.m_fi)

		//curNode.initialize()
	}
	void NBN_GridTreeDivision::udpateSubspace(std::shared_ptr<TreeNode>& curTreeNode, int curIdx) {
		int idx = curIdx;
		auto& sonNode(curTreeNode->m_division_nodes[idx]);
		if (judgeDivisionFlag(sonNode)) {
			divideSubSpace(curTreeNode, idx);
			auto& sonTreeNode = curTreeNode->m_trees[idx];
			sonNode.m_representative_solId = sonTreeNode->m_best_solId;
		}
		else {
			sonNode.m_representative_solId = updateLocalNetwork(sonNode.m_sample_solIds);
		}
	}
	int NBN_GridTreeDivision::updateLocalNetwork(const std::vector<int>& solIdxs) {
		std::vector<int> sortedIds(solIdxs);
		std::sort(sortedIds.begin(), sortedIds.end(), [&](int a,int b) {
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
					if (cur_dis < m_dis2parent[cur_solId]|| (cur_dis == m_dis2parent[cur_solId]&& m_random->uniform.next()<0.5)) {
						m_dis2parent[cur_solId] = cur_dis;
						m_belong[cur_solId] = better_solId;
					}
				}
			}
			
		}



		return sortedIds.back();
	}



	void NBN_GridTreeDivision::insertNewSolsIntoTreeNode(TreeNode& curTreeNode, const std::vector<int>& curIdxs) {
		int belongNodeId(0);
		for (auto& curId : curIdxs) {
			auto& curSol(m_sols[curId]);
			getSolInNodeId(*curSol, curTreeNode, belongNodeId);
			curTreeNode.m_division_nodes[belongNodeId].m_sample_solIds.push_back(curId);
		}
	}
	

	void NBN_GridTreeDivision::calculateNetworkAccurate(TreeNode& curTreeNode) {
		std::vector<int> sorted_idx(curTreeNode.m_division_nodes.size());
		for (int idx(0); idx < sorted_idx.size(); ++idx) {
			sorted_idx[idx] = idx;
		}
		auto& nbn_nodes(curTreeNode.m_division_nodes);
		std::sort(sorted_idx.begin(), sorted_idx.end(), [&](
			int a, int  b
			) {
			return m_fitness[nbn_nodes[a].m_representative_solId] <
				m_fitness[nbn_nodes[b].m_representative_solId];
		});

		curTreeNode.m_cur_visited = 0;
		for (auto& curNode : curTreeNode.m_division_nodes) {
			curNode.m_better_range_nodeId.clear();
			curNode.m_better_range_nodeId.push_back(curNode.m_node_id);
		}
		for (int sidx(0); sidx+1 < sorted_idx.size(); ++sidx) {
			//std::cout << "curId\t" << sidx << std::endl;
			auto sortedId = sorted_idx[sidx];
			auto& cur_node(nbn_nodes[sortedId]);

			updateNeighborAndParent(curTreeNode, cur_node);

		}
		curTreeNode.m_best_divNodeId = sorted_idx.back();
		curTreeNode.m_best_solId = nbn_nodes[sorted_idx.back()].m_representative_solId;
	}


	void NBN_GridTreeDivision::calculateNetwork(TreeNode& curTreeNode) {
		std::vector<int> sorted_idx(curTreeNode.m_division_nodes.size());
		for (int idx(0); idx < sorted_idx.size(); ++idx) {
			sorted_idx[idx] = idx;
		}
		auto& nbn_nodes(curTreeNode.m_division_nodes);
		std::sort(sorted_idx.begin(), sorted_idx.end(), [&](
			int a, int  b
			) {
			return m_fitness[nbn_nodes[a].m_representative_solId] <
				m_fitness[nbn_nodes[b].m_representative_solId];
		});

		curTreeNode.m_cur_visited = 0;
		for (auto& curNode : curTreeNode.m_division_nodes) {
			curNode.m_better_range_nodeId.clear();
		}
		std::vector<int> peak_ranges;

		for (int sidx(0); sidx < sorted_idx.size();++sidx) {
			std::cout << "curId\t" << sidx << std::endl;
			auto sortedId = sorted_idx[sidx];
			auto& cur_node(nbn_nodes[sortedId]);

			//if (sortedId == 1533708) {
			//	int stop = -1;
			//}

			getNeiborPeakRecallFun(curTreeNode, cur_node, peak_ranges);

			if (peak_ranges.empty()&& sortedId!= sorted_idx.back()) {

				std::cout << "nbn error\tcurIdx\t"<< sortedId <<"\tat\t"<<m_environment->problem()->name() <<"\t dim\t"<<m_environment->problem()->numberVariables() << std::endl;
				std::vector<int> curVisited(m_sols.size() + 1,-1);
				std::vector<double> peak_ranges_dis;
				double curDis = 0;
				Node* par_node = nullptr;
				auto& cur_sol(m_sols[cur_node.m_representative_solId]);
				for (int idx(sidx + 1); idx < sorted_idx.size(); ++idx) {
					auto& nei_node(nbn_nodes[sorted_idx[idx]]);
					curTreeNode.findParent(&nei_node, par_node);
					auto& nei_sol(m_sols[nei_node.m_representative_solId]);
					curDis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), nei_sol->variableBase());
					if (curVisited[par_node->m_node_id] == -1) {
						curVisited[par_node->m_node_id] = peak_ranges.size();
						peak_ranges.push_back(nei_node.m_node_id);
						peak_ranges_dis.push_back(curDis);
					}
					else {
						int curViId = curVisited[par_node->m_node_id];
						if (curDis < peak_ranges_dis[curViId]) {
							peak_ranges_dis[curViId] = curDis;
							peak_ranges[curViId] = nei_node.m_node_id;
						}
						else if (curDis == peak_ranges_dis[curViId] && m_random->uniform.next() < 0.5) {
							peak_ranges[curViId] = nei_node.m_node_id;
						}
					}
				}
			}

			cur_node.m_better_range_nodeId.clear();

			double min_dis = std::numeric_limits<double>::max();
			double cur_dis(0);
			m_random->uniform.shuffle(peak_ranges.begin(), peak_ranges.end());
			for (auto& nei_idx : peak_ranges) {
				auto& nei_node(nbn_nodes[nei_idx]);

				

				{
					auto& cur_sol(m_sols[cur_node.m_representative_solId]);
					auto& nei_sol(m_sols[nei_node.m_representative_solId]);
					cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(),nei_sol->variableBase());
					//cur_node.m_dis2parent = cur_sol->normalizedVariableDistance(*par_sol, m_id_pro);
				}
				if (cur_node.m_representative_solId == nei_node.m_representative_solId) {
					continue;
				}

				//cur_dis = m_environment->problem()->normalizedVariableDistance(*cur_node.m_sol, *nei_node.m_sol);
				//cur_dis = m_environment->problem()->normalizedVariableDistance(*m_cur_sols[cur_node.m_sol_id], *m_cur_sols[m_nbn_nodes[nei_idx].m_sol_id]);
				//cur_dis = m_cur_sols[cur_node.m_sol_id]->normalizedVariableDistance(*m_cur_sols[m_nbn_nodes[nei_idx].m_sol_id], m_id_pro);
				if (cur_dis < min_dis) {
					min_dis = cur_dis;
					if (cur_node.m_parent_nodeId != cur_node.m_node_id) {
					//	cur_node.m_better_range_nodeId.push_back(cur_node.m_parent_nodeId);
					}
					cur_node.m_parent_nodeId = nei_idx;
				}
				else if (cur_dis == min_dis && m_random->uniform.next() < 0.5) {
					min_dis = cur_dis;
					if (cur_node.m_parent_nodeId != cur_node.m_node_id) {
				//		cur_node.m_better_range_nodeId.push_back(cur_node.m_parent_nodeId);
					}
					cur_node.m_parent_nodeId = nei_idx;
				}
				else {
					//cur_node.m_better_range_nodeId.push_back(nei_idx);
				}
			}
			cur_node.m_better_range_nodeId = peak_ranges;
			
			cur_node.m_direct_parent_nodeId = cur_node.m_parent_nodeId;

			{
				int curSolId = cur_node.m_representative_solId;
				int parSolId = nbn_nodes[cur_node.m_direct_parent_nodeId].m_representative_solId;
				if (curSolId != parSolId) {
					auto& cur_sol(m_sols[curSolId]);
					auto& par_sol(m_sols[parSolId]);
					m_dis2parent[curSolId] = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), par_sol->variableBase());
					//cur_node.m_dis2parent = cur_sol->normalizedVariableDistance(*par_sol, m_id_pro);
					m_belong[curSolId] = parSolId;
				}

			}
		}


		//// for test
		//{
		//	for (int idx(0); idx < m_belong.size(); ++idx) {
		//		if (m_fitness[idx] > m_fitness[m_belong[idx]]) {
		//			int stop = -1;
		//			std::cout << "error" << std::endl;
		//		}
		//	}
		//}
		//
		//for (auto& node_iter : nbn_nodes) {
		//	m_belong[node_iter.m_representative_solId] = nbn_nodes[node_iter.m_direct_parent_nodeId].m_representative_solId;
		//	m_dis2parent[node_iter.m_representative_solId] = node_iter.m_dis2parent;
		//}
		curTreeNode.m_best_divNodeId = sorted_idx.back();
		curTreeNode.m_best_solId = nbn_nodes[sorted_idx.back()].m_representative_solId;
	}


	void NBN_GridTreeDivision::assignedSols(int numSols) {
		int originSize(m_sols.size());
		m_sols.resize(m_sols.size() + numSols);
		m_belong.resize(m_sols.size(),-1);
		m_fitness.resize(m_sols.size(),0);
		m_dis2parent.resize(m_sols.size(),std::numeric_limits<double>::max());
		m_flagOpt.resize(m_sols.size(), false);
		m_popIter.resize(m_sols.size(),-1);
		m_popSolId.resize(m_sols.size(),-1);
		m_algId.resize(m_sols.size(), -1);

		for (int idx(originSize); idx < m_belong.size(); ++idx) {
			m_belong[idx] = idx;
		}
	}
	void NBN_GridTreeDivision::updateFitnessThreadTask(int from, int to) {
		for (size_t idx(from); idx < to; ++idx) {
			m_eval_fun(*m_sols[idx], m_environment.get());
			m_fitness[idx] = m_sols[idx]->fitness();
		}
	}


	void NBN_GridTreeDivision::updateFitness() {
		if (!m_flag_multiThread ||!m_flag_evaluate_multitask) {
			updateFitnessThreadTask(m_solsFitnessUpdatedIdxs, m_sols.size());
		}
		else {
			std::vector<std::thread> thrds;
			int num_task = std::thread::hardware_concurrency();
			int num_samples = m_sols.size()-m_solsFitnessUpdatedIdxs;
			std::vector<int> tasks;
			UTILITY::assignThreads(num_samples, num_task, tasks);
			std::pair<int, int> from_to;
			for (size_t i = 0; i < num_task; ++i) {
				from_to.first = tasks[i]+m_solsFitnessUpdatedIdxs;
				from_to.second = tasks[i + 1]+ m_solsFitnessUpdatedIdxs;
				thrds.push_back(std::thread(
					&NBN_GridTreeDivision::updateFitnessThreadTask, this,
					from_to.first, from_to.second));
			}
			for (auto& thrd : thrds)
				thrd.join();
		}
		m_solsFitnessUpdatedIdxs = m_sols.size();
	}



	void NBN_GridTreeDivision::getSolInNodeId(const SolutionBase& new_sol, const TreeNode& node,int& belong_id) {
		std::vector<int> cur_vec;
		solToVec(new_sol, cur_vec,node.m_dim_div,node.m_range_div,node.m_range_from);
		vecToIdx(cur_vec, belong_id,node.m_dim_div);
	}
	void NBN_GridTreeDivision::getSolId(const SolutionBase& new_sol, std::vector<int>& nodePath) {
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



	void NBN_GridTreeDivision::insertSol(const SolutionBase& new_sol, std::shared_ptr<TreeNode>& targetTreeNode,
		int& belong_idx,
		bool flagOpt ,
		int popIter , int popSolId , int algId 
	) {
		std::shared_ptr<SolutionBase> cur_sol(m_environment->problem()->createSolution(new_sol));
		m_eval_fun(*cur_sol, m_environment.get());
		assignedSols(1);
		m_sols.back() = cur_sol;
		m_fitness.back() = cur_sol->fitness();
		m_flagOpt.back() = flagOpt;
		m_popIter.back() = popIter;
		m_popSolId.back() = popSolId;
		m_algId.back() = algId;
		int cur_solId = m_sols.size() - 1;

		targetTreeNode = m_root;
		//int rootBelong = -1;
		while (true) {
			getSolInNodeId(*cur_sol, *targetTreeNode, belong_idx);
			if (targetTreeNode->m_trees[belong_idx] != nullptr) {
				targetTreeNode = targetTreeNode->m_trees[belong_idx];
			}
			else break;
		}
		targetTreeNode->m_division_nodes[belong_idx].m_sample_solIds.push_back(cur_solId);
	}

	void NBN_GridTreeDivision::insertSol(const SolutionBase& new_sol, int& belong_id,
		bool flagOpt,
		int popIter, int popSolId , int algId ) {
		std::shared_ptr<TreeNode> targetTreeNode = nullptr;
		insertSol(new_sol, targetTreeNode, belong_id, flagOpt,popIter, popSolId, algId);

	}
	void NBN_GridTreeDivision::insertSolAndUpdateTree(const SolutionBase& new_sol, int& belong_id, 
		bool flagOpt,
		int popIter , int popSolId , int algId ) {
		std::shared_ptr<TreeNode> targetTreeNode = nullptr;
		insertSol(new_sol, targetTreeNode, belong_id, flagOpt, popIter,popSolId,algId);
		int originSolId(targetTreeNode->m_division_nodes[belong_id].m_representative_solId);
		udpateSubspace(targetTreeNode, belong_id);
		if (originSolId != targetTreeNode->m_division_nodes[belong_id].m_representative_solId) {
			updateTreeFromBottom(targetTreeNode, belong_id);
		}
		
	}

	void NBN_GridTreeDivision::updateTreeFromBottom(std::shared_ptr<TreeNode> curTreeNode, int changeNodeId) {

		//while (curTreeNode != nullptr) {
		//	int
		//}

		while (curTreeNode!=nullptr) {
			auto& curNode(curTreeNode->m_division_nodes[changeNodeId]);
			curNode.m_parent_nodeId = curNode.m_direct_parent_nodeId = curNode.m_node_id;

			int curSolId = curNode.m_representative_solId;
			auto& curSol(m_sols[curSolId]);
			auto& curFit(m_fitness[curSolId]);
			auto& curDisToPar(m_dis2parent[curSolId]);
			auto& curBelongId(m_belong[curSolId]);

			for (auto& neiNode : curTreeNode->m_division_nodes) {
				if (neiNode.m_node_id != curNode.m_node_id) {
					int neiSolId = neiNode.m_representative_solId;
					auto& neiSol(m_sols[neiSolId]);
					double neiFit(m_fitness[neiSolId]);
					auto& neiDisToPar(m_dis2parent[neiSolId]);
					auto& neiBelongId(m_belong[neiSolId]);
					double curDis(m_environment->problem()->normalizedVariableDistance(curSol->variableBase(), neiSol->variableBase()));
					if (curFit > neiFit) {
						if (curDis < neiDisToPar || (curDis == neiDisToPar && (m_random->uniform.next() < 0.5))) {
							neiNode.m_direct_parent_nodeId = curNode.m_node_id;
							neiDisToPar = curDis;
							neiBelongId = curSolId;
						}
					}
					else if (curFit < neiFit) {
						if (curDis < curDisToPar || (curDis == curDisToPar && (m_random->uniform.next() < 0.5))) {
							curNode.m_direct_parent_nodeId = neiNode.m_node_id;
							curDisToPar = curDis;
							curBelongId = neiSolId;
						}
					}
				}
			}

			while (curTreeNode->m_best_divNodeId != curTreeNode->m_division_nodes[curTreeNode->m_best_divNodeId].m_direct_parent_nodeId) {
				curTreeNode->m_best_divNodeId = curTreeNode->m_division_nodes[curTreeNode->m_best_divNodeId].m_direct_parent_nodeId;
			}

			if (curTreeNode->m_best_solId != curTreeNode->m_division_nodes[curTreeNode->m_best_divNodeId].m_representative_solId) {
				curTreeNode->m_best_solId = curTreeNode->m_division_nodes[curTreeNode->m_best_divNodeId].m_representative_solId;
				changeNodeId = curTreeNode->m_son_nodeId;
				curTreeNode = curTreeNode->m_parent;

			}
			else break;
		}

	}

}

void ofec::NBN_GridTreeDivision::initialize_(bool flag_grid_sample)
{
	m_dim = m_environment->problem()->numberVariables();
	const auto& boundary = CAST_CONOP(m_environment->problem())->boundary();
	m_root.reset(new TreeNode);
	divideSpace(m_root, boundary, m_maxSample);
//	updateMultiOpt();	
}

void ofec::NBN_GridTreeDivision::updateGlobalNetwork() {
	for (int idx(0); idx < m_root->m_division_nodes.size(); ++idx) {
		udpateSubspace(m_root, idx);
	}
	//calculateNetwork(*m_root);
	calculateNetworkAccurate(*m_root);
//	updateMultiOpt();
}
