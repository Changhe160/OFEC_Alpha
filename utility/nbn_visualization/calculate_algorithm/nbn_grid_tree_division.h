

#ifndef NBN_GRID_DIVISION_TREE_H
#define NBN_GRID_DIVISION_TREE_H

#include <vector>
#include <memory>
#include <queue>

#include "../../../core/problem/solution.h"
#include "nbn_division_base.h"


//#include "core/problem/solution.h"
//#include "utility/typevar/typevar.h"



namespace ofec {
	class NBN_GridTreeDivision : public NBN_DivisionBase {
	public:
		using SolutionType = SolutionBase;
		using SolContinousType = Solution<>;
		struct Node {

			int m_node_id = -1;
			int m_representative_solId = -1;
			//double m_dis2parent = 0;
			std::vector<int> m_vec;
			unsigned long long m_visited_stamp = 0;
			unsigned long long m_visited_stamp2 = 0;
			std::vector<int> m_neighbor_nodeIds;
			//std::vector<std::pair<int, double>> m_neighbor_nodeId_dis;
			int m_parent_nodeId = -1;
			int m_direct_parent_nodeId = -1;
			std::vector<int> m_better_range_nodeId;
			std::vector<int> m_sample_solIds;

			void clear() {
				m_node_id = -1;
				m_representative_solId = -1;
				//m_parent_solId = -1;
			//	m_dis2parent = 0;
				m_vec.clear();
				m_visited_stamp = 0;
				m_neighbor_nodeIds.clear();
			//	m_neighbor_nodeId_dis.clear();
				m_parent_nodeId = -1;
				m_direct_parent_nodeId = -1;
				m_better_range_nodeId.clear();
				m_sample_solIds.clear();
			}
			void initialize(int id) {
				clear();
				m_node_id = m_direct_parent_nodeId = m_parent_nodeId = id;
			//	m_dis2parent = std::numeric_limits<double>::max();
			}
		};

		struct TreeNode {

			int m_son_nodeId = -1;
			std::shared_ptr<TreeNode> m_parent = nullptr;

			int m_best_solId = -1;
			int m_best_divNodeId = -1;

			int m_numberDivision = 0;
			unsigned long long m_cur_visited = 0;
			unsigned long long m_cur_visited2 = 0;
			//std::vector<double> m_center;
			std::vector<std::pair<double, double>> m_boundary;

			std::vector<int> m_dim_div;
			std::vector<double> m_range_div;
			std::vector<double> m_range_from;

			// neighbor info
			std::vector<std::array<int, 2>> m_neighbors;
			std::vector<Node> m_division_nodes;
			std::vector<std::shared_ptr<TreeNode>> m_trees;


			void resizeNumberDivision(int numDiv);

			void resetVisited() {
				m_cur_visited = 0;
				for (auto& it : m_division_nodes) it.m_visited_stamp = m_cur_visited;
			}

			void resetVisited2() {
				m_cur_visited2 = 0;
				for (auto& it : m_division_nodes) it.m_visited_stamp = m_cur_visited;
			}

			void clear() {
				m_neighbors.clear();
				m_division_nodes.clear();
				m_trees.clear();
			}

			void initialize(const std::vector<std::pair<double, double>> &boundary, int sampleSize);

			void getSubSpace(int nodeId, std::vector<std::pair<double, double>>& boundary);

			void mergeParent(Node* curNode);


			void findParent(Node* curNode,Node*&parent);
			
			
		};

	protected:

		int m_dim = 2;

		std::shared_ptr<TreeNode> m_root;
		int m_solsFitnessUpdatedIdxs = 0;
		
		//// updateNodeInfo
		//std::vector<int> m_node_paths;
		//bool m_flag_node_division = false;
		int m_division_threadhold = 100;



		std::vector<std::shared_ptr<SolutionType>> m_added_sols;
		std::vector<std::shared_ptr<SolutionType>> m_sols;
		std::vector<double> m_fitness;
		std::vector<int> m_belong;
		std::vector<double> m_dis2parent;
		std::vector<bool> m_flagOpt;
		std::vector<int> m_popIter;
		std::vector<int> m_popSolId;
		std::vector<int> m_algId;

		double m_filterDis = 0.02;


		// nbn info
	protected:


		void updateNeighborInfo(int idx, int dim, std::vector<Node>& division_nodes, const std::vector<int>& m_dim_div) {
			std::vector<int> neiVec;
			int neiIdx(0);

			std::vector<int> curVec(dim, 0);
			int curIdx(0);
			/*for (int idx(0); idx < m_numberDivision; ++idx) */{
				auto& cur_node = division_nodes[idx];
				idxToVec(idx, curVec, m_dim_div);
				vecToIdx(curVec, curIdx, m_dim_div);
				neiVec = curVec;
				for (int idDim(0); idDim < dim; ++idDim) {
					--neiVec[idDim];
					if (neiVec[idDim] >= 0) {
						vecToIdx(neiVec, neiIdx, m_dim_div);
					//	auto& nei_node = division_nodes[neiIdx];
						cur_node.m_neighbor_nodeIds.push_back(neiIdx);
					//	nei_node.m_neighbor_nodeIds.push_back(curIdx);
						//			m_neighbors.push_back({ idx, neiIdx });
					}					
					++neiVec[idDim];

					++neiVec[idDim];

					if (neiVec[idDim] < dim) {
						vecToIdx(neiVec, neiIdx, m_dim_div);
						//	auto& nei_node = division_nodes[neiIdx];
						cur_node.m_neighbor_nodeIds.push_back(neiIdx);
						//	nei_node.m_neighbor_nodeIds.push_back(curIdx);
							//			m_neighbors.push_back({ idx, neiIdx });
					}

					--neiVec[idDim];
				}
			}
		}


		bool judgeDivisionFlag(const Node& cur) {
			return cur.m_sample_solIds.size() >= m_division_threadhold;
		}
		void updateFitnessThreadTask(int from, int to);

		void assignedSols(int numSols);

		void insertSol(const SolutionBase& new_sol, 
			std::shared_ptr<TreeNode>& targetTreeNode, 
			int& belong_idx,
			bool flagOpt = false,
			int popIter = -1, int popSolId = -1, int algId = -1);
		


		void divideSpace(std::shared_ptr<TreeNode>&, const std::vector<std::pair<double, double>>& boundary, int sampleSize);
		void udpateSubspace(std::shared_ptr<TreeNode>&, int curIdx);
		void divideSubSpace(std::shared_ptr<TreeNode>&, int curIdx);
		void generateSolution(TreeNode& curNode);
		void updateFitness();
		void calculateNetwork(TreeNode& curNode);
		void calculateNetworkAccurate(TreeNode& curNode);
		int  updateLocalNetwork(const std::vector<int>& solIdxs);
		void insertNewSolsIntoTreeNode(TreeNode& curTreeNode, const std::vector<int>& curIdxs);
		void updateTreeFromBottom(std::shared_ptr<TreeNode> curTreeNode, int changeNodeId);
		void getSolId(const SolutionBase& new_sol, std::vector<int>& nodeIds);




		void insertSol(const SolutionBase& new_sol, int& belong_id, bool flagOpt = false, int popIter = -1, int popSolId = -1, int algId = -1);
		void insertSolAndUpdateTree(const SolutionBase& new_sol, int& belong_id, bool flagOpt = false, int popIter = -1, int popSolId = -1, int algId = -1);


		virtual void addSol(const SolutionBase& new_sol, int& belong_id, bool flag_opt = false,
			int popIter = -1, int popSolId = -1, int algId = -1) override {
			insertSol(new_sol, belong_id, flag_opt, popIter, popSolId, algId);
		}
	public:

		virtual void initialize_(bool flag_grid_sample = true) override;


		static void vecToIdx(const std::vector<int>& cur, int& idx, const std::vector<int>& dim_div);
		static void solToVec(const SolutionBase& sol, std::vector<int>& vec, const std::vector<int>& dim_div,
			const std::vector<double>& range_div,
			const std::vector<double>& range_from);
		static void idxToVec(int idx, std::vector<int>& vec, const std::vector<int>& dim_div);
		static void getSolInNodeId(const SolutionBase& new_sol, const TreeNode& node, int& belong_id);

	public:

		struct NodeDisStruct {
			int m_cur_id = 0;
			Node* m_cur = nullptr;
			double m_cur_dis = 0;
			bool operator<(const NodeDisStruct& a) const
			{
				return m_cur_dis > a.m_cur_dis;
			}
		};

		void updateNeighborAndParent(TreeNode& curTreeNode, Node& cur_node) {
			if (++curTreeNode.m_cur_visited == 0) {
				curTreeNode.resetVisited();
			}
			if (++curTreeNode.m_cur_visited2 == 0) {
				curTreeNode.resetVisited2();
			}
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			cur_node.m_visited_stamp = curTreeNode.m_cur_visited;
			cur_node.m_visited_stamp2 = curTreeNode.m_cur_visited2;

			auto& cur_sol(m_sols[cur_node.m_representative_solId]);

			std::priority_queue<NodeDisStruct> nei_que;
			NodeDisStruct curQueNode;
			for (auto& nei_info : cur_node.m_neighbor_nodeIds) {
				auto& nei_idx = nei_info;
				auto nei_node(&nbn_nodes[nei_idx]);
				if (nei_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
					nei_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
					curQueNode.m_cur = nei_node;
					curQueNode.m_cur_id = nei_idx;
					{
						auto& nei_sol(m_sols[nei_node->m_representative_solId]);
						curQueNode.m_cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), nei_sol->variableBase());
						//cur_node.m_dis2parent = cur_sol->norVariableDistance(*par_sol, m_id_pro);
					}
					nei_que.push(curQueNode);
				}
			}


			std::vector<Node*> attraction;
			while (!nei_que.empty()) {
				curQueNode = nei_que.top();
				auto nei_node = curQueNode.m_cur;
				if(m_belong[nei_node->m_representative_solId]!= nei_node->m_representative_solId)
				/*if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId])*/ {
					nei_que.pop();
					attraction.push_back(curQueNode.m_cur);
					for (auto& son_idx : nei_node->m_better_range_nodeId) {
						auto son_node(&nbn_nodes[son_idx]);
						if (son_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
							son_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
							curQueNode.m_cur = son_node;
							curQueNode.m_cur_id = son_idx;
							{
					
								auto& son_sol(m_sols[son_node->m_representative_solId]);
								curQueNode.m_cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), son_sol->variableBase());
								//cur_node.m_dis2parent = cur_sol->norVariableDistance(*par_sol, m_id_pro);
							}
							nei_que.push(curQueNode);
						}
					}
				}
				else break;
			}
			{
				//cur_node.m_better_range_nodeId = peak_ranges;
				cur_node.m_better_range_nodeId.clear();
				NodeDisStruct parentNode = nei_que.top();
				while (!nei_que.empty()) { 
					curQueNode = nei_que.top();
					nei_que.pop();
					cur_node.m_better_range_nodeId.push_back(curQueNode.m_cur_id);
				}
				cur_node.m_direct_parent_nodeId = cur_node.m_parent_nodeId = parentNode.m_cur_id;
				{
					int curSolId = cur_node.m_representative_solId;
					int parSolId = nbn_nodes[cur_node.m_direct_parent_nodeId].m_representative_solId;
					m_dis2parent[curSolId] = parentNode.m_cur_dis;
					m_belong[curSolId] = parSolId;

				}
			}
/*
			if (attraction.size() * cur_node.m_better_range_nodeId.size() < 1e6) {
				std::vector<int> nei_peak;
				for (auto it = attraction.rbegin(); it != attraction.rend(); ++it) {
					auto& nei_node = *it;
					if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId]) {
						nei_peak.clear();
						if (++curTreeNode.m_cur_visited2 == 0) {
							curTreeNode.resetVisited2();
						}
						for (auto& son_idx : nei_node->m_better_range_nodeId) {
							auto son_node(&nbn_nodes[son_idx]);
							if (m_fitness[cur_node.m_representative_solId] > m_fitness[son_node->m_representative_solId]) {
								for (auto& peakId : son_node->m_better_range_nodeId) {
									auto better_node(&nbn_nodes[peakId]);
									if (better_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
										better_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
										nei_peak.push_back(peakId);
									}

								}
							}
							else {
								if (son_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
									son_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
									nei_peak.push_back(son_idx);
								}
							}

						}
						swap(nei_peak, nei_node->m_better_range_nodeId);
					}
				}
			}
	*/	}

		void getNeiborPeakQueue(TreeNode& curTreeNode, Node& cur_node, std::vector<int>& peak_ranges) {
			if (++curTreeNode.m_cur_visited == 0) {
				curTreeNode.resetVisited();
			}
			if (++curTreeNode.m_cur_visited2 == 0) {
				curTreeNode.resetVisited2();
			}
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			peak_ranges.clear();
			cur_node.m_visited_stamp = curTreeNode.m_cur_visited;

			std::queue<Node*> nei_que;
			std::vector<Node*> attraction;
			for (auto& nei_info : cur_node.m_neighbor_nodeIds) {
				auto& nei_idx = nei_info;
				auto nei_node(&nbn_nodes[nei_idx]);
				/*if (nei_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) */{
					nei_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
					nei_que.push(nei_node);
					attraction.push_back(nei_node);
				}
			}


			while (!nei_que.empty()) {
				Node* nei_node = nei_que.front();
				//son_node
				nei_que.pop();
				if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId]) {
					for (auto& son_idx : nei_node->m_better_range_nodeId) {
						auto son_node(&nbn_nodes[son_idx]);
						if (son_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
							son_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
							nei_que.push(son_node);
							attraction.push_back(son_node);
						}
					}
				}
				else {
					if (nbn_nodes[nei_node->m_node_id].m_visited_stamp != curTreeNode.m_cur_visited) {
						nbn_nodes[nei_node->m_node_id].m_visited_stamp = curTreeNode.m_cur_visited;
						peak_ranges.push_back(nei_node->m_node_id);
					}
				}
			}
			std::vector<int> nei_peak;
			for (auto it = attraction.rbegin(); it != attraction.rend(); ++it) {
				auto& nei_node = *it;
				if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId]) {
					nei_peak.clear();
					if (++curTreeNode.m_cur_visited2 == 0) {
						curTreeNode.resetVisited2();
					}
					for (auto& son_idx : nei_node->m_better_range_nodeId) {
						auto son_node(&nbn_nodes[son_idx]);
						if (m_fitness[cur_node.m_representative_solId] > m_fitness[son_node->m_representative_solId]) {
							for (auto& peakId : son_node->m_better_range_nodeId) {
								auto better_node(&nbn_nodes[peakId]);
								if (better_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
									better_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
									nei_peak.push_back(peakId);
								}

							}
						}
						else {
							if (son_node->m_visited_stamp2 != curTreeNode.m_cur_visited2) {
								son_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
								nei_peak.push_back(son_idx);
							}
						}
		
					}
					swap(nei_peak, nei_node->m_better_range_nodeId);
				}
			}

			
		}

		void getNeiborPeakRecallFun(TreeNode& curTreeNode, Node& cur_node, std::vector<int>& peak_ranges) {
			if (++curTreeNode.m_cur_visited == 0) {
				curTreeNode.resetVisited();
			}
			if (++curTreeNode.m_cur_visited2 == 0) {
				curTreeNode.resetVisited2();
			}
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			peak_ranges.clear();
			cur_node.m_visited_stamp = curTreeNode.m_cur_visited;

			for (auto& nei_info : cur_node.m_neighbor_nodeIds) {
				auto& nei_idx = nei_info;
				auto nei_node(&nbn_nodes[nei_idx]);

				getBetterNeighbor(curTreeNode, cur_node, nei_node, peak_ranges);

				//curTreeNode.mergeParent(nei_node);
				//if (nei_node->m_parent_nodeId == cur_node.m_node_id) {
				//	for (auto& son_idx : nei_node->m_better_range_nodeId) {
				//		auto son_node(&nbn_nodes[son_idx]);
				//		curTreeNode.mergeParent(son_node);
				//		if (nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp != curTreeNode.m_cur_visited) {
				//			nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp = curTreeNode.m_cur_visited;
				//			peak_ranges.push_back(son_node->m_parent_nodeId);
				//		}
				//	}
				//}
				//else {
				//	if (nbn_nodes[nei_node->m_parent_nodeId].m_visited_stamp != curTreeNode.m_cur_visited) {
				//		nbn_nodes[nei_node->m_parent_nodeId].m_visited_stamp = curTreeNode.m_cur_visited;
				//		peak_ranges.push_back(nei_node->m_parent_nodeId);
				//	}
				//}
			}
		}




		void getBetterNeighbor2(TreeNode& curTreeNode, const Node& cur_node, Node* nei_node, std::vector<int>& peak_ranges) {
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			if (nei_node->m_visited_stamp2 == curTreeNode.m_cur_visited2)return;
			nei_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
			if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId] ) {
				for (auto& son_idx : nei_node->m_better_range_nodeId) {
					auto son_node(&nbn_nodes[son_idx]);
					getBetterNeighbor2(curTreeNode, cur_node, son_node, peak_ranges);
					//
					//curTreeNode.mergeParent(son_node);
					//if (nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp != curTreeNode.m_cur_visited) {
					//	nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp = curTreeNode.m_cur_visited;
					//	peak_ranges.push_back(son_node->m_parent_nodeId);
					//}
				}
			}
			else {
				if (nbn_nodes[nei_node->m_node_id].m_visited_stamp != curTreeNode.m_cur_visited) {
					nbn_nodes[nei_node->m_node_id].m_visited_stamp = curTreeNode.m_cur_visited;
					peak_ranges.push_back(nei_node->m_node_id);
				}
			}
		}



		void getBetterNeighbor(TreeNode& curTreeNode, const Node & cur_node,  Node* nei_node, std::vector<int> & peak_ranges) {
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			curTreeNode.mergeParent(nei_node);
			if (nei_node->m_visited_stamp2 == curTreeNode.m_cur_visited2)return;
			nei_node->m_visited_stamp2 = curTreeNode.m_cur_visited2;
			if (nei_node->m_parent_nodeId == cur_node.m_node_id) {
				for (auto& son_idx : nei_node->m_better_range_nodeId) {
					auto son_node(&nbn_nodes[son_idx]);
					getBetterNeighbor(curTreeNode, cur_node, son_node, peak_ranges);
					//
					//curTreeNode.mergeParent(son_node);
					//if (nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp != curTreeNode.m_cur_visited) {
					//	nbn_nodes[son_node->m_parent_nodeId].m_visited_stamp = curTreeNode.m_cur_visited;
					//	peak_ranges.push_back(son_node->m_parent_nodeId);
					//}
				}
			}
			else {
				if (nbn_nodes[nei_node->m_parent_nodeId].m_visited_stamp != curTreeNode.m_cur_visited) {
					nbn_nodes[nei_node->m_parent_nodeId].m_visited_stamp = curTreeNode.m_cur_visited;
					peak_ranges.push_back(nei_node->m_parent_nodeId);
				}
			}
		}


			
		void updateMultiOpt() {
			/*
			* 		auto& cur_sol(m_sols[curSolId]);
				auto& par_sol(m_sols[parSolId]);
				m_dis2parent[curSolId] = m_environment->problem()->norVariableDistance(*cur_sol, *par_sol);
		
			*/
			std::vector<int> optIds;
			for (int idx(0); idx < m_sols.size(); ++idx) {
				if (m_belong[idx] == idx) {
					optIds.push_back(idx);
				}
			}
			if (optIds.size() >= 2) {
				m_random->uniform.shuffle(optIds.begin(), optIds.end());

				int curId = optIds.front();
				auto& cur_sol(m_sols[curId]);
				for (int idx(1); idx < optIds.size(); ++idx) {
					int newId = optIds[idx];
					auto& new_sol(m_sols[optIds[idx]]);
					double dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), new_sol->variableBase());
					if (m_dis2parent[newId] > dis|| (m_dis2parent[newId] == dis && m_random->uniform.next()<0.5 )) {
						m_dis2parent[newId] = dis;
						m_belong[newId]=curId;
					}
					if (m_dis2parent[curId] > dis || (m_dis2parent[curId] == dis && m_random->uniform.next() < 0.5)) {
						m_dis2parent[curId] = dis;
						m_belong[curId] = newId;
					}
					if (m_random->uniform.next() < 0.5) {
						curId = newId;
					}
				}
				m_belong[curId] = curId;
			}
		}

		NBN_GridTreeDivision() = default;
		~NBN_GridTreeDivision() {
			if (m_root != nullptr) {
				m_root->m_trees.clear();
			}

			m_root = nullptr;
		}
		virtual size_t size()  const  override {
			return m_sols.size();
		}

		void updateGlobalNetwork();





		void addSol(const SolutionBase& new_sol) {
			std::shared_ptr<SolutionBase> cur_sol(m_environment->problem()->createSolution(new_sol));
			m_added_sols.emplace_back(cur_sol);
			
		}
		
		int getSolNodeId(const SolutionBase& sol) {
			int nodeId(-1);
			getSolInNodeId(sol, *m_root, nodeId);
			return nodeId;
		}

		const SolutionBase* getSol(int solId) const override;



		virtual void addRandom(int numSample) {
			using namespace ofec;
			SolContinousType sol(m_environment->problem()->numberObjectives(),
				m_environment->problem()->numberConstraints(),
				m_environment->problem()->numberVariables());
			for (int idx(0); idx < numSample; ++idx) {
				sol.initialize(m_environment.get(), m_random.get());
				int belong_id(0);
				addSol(sol, belong_id);
			}


		}
		//void updateDivision();
		//void updateDivision(std::shared_ptr<TreeNode>&, const std::vector<std::pair<double, double>>& boundary, int sampleSize);
		//
		


		void getNearestBetterNetworkShareMemory(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2par) {
			sols = m_sols;
			fitness = m_fitness;
			belong = m_belong;
			dis2par = m_dis2parent;
		}
		void getNearestBetterNetworkShareMemory(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2par,
			std::vector<int> popIters,
			std::vector<int> popSolIds,
			std::vector<int> algIds
			) {
			sols = m_sols;
			fitness = m_fitness;
			belong = m_belong;
			dis2par = m_dis2parent;
			popIters = m_popIter;
			popSolIds = m_popSolId;
			algIds = m_algId;
		}

		// belong[idx]==idx  peakInfo
		virtual void getSharedNBN(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2par,
			std::vector<bool>& flagOpt
		)const override {
			sols = m_sols;
			fitness = m_fitness;
			belong = m_belong;
			dis2par = m_dis2parent;
			flagOpt = m_flagOpt;
		}

		// belong[idx]==idx  peakInfo
		virtual void getSharedNBN(
			std::vector<std::shared_ptr<SolutionBase>>& sols,
			std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			std::vector<int> popIters,
			std::vector<int> popSolIds,
			std::vector<int> algIds
		)const override {
			sols = m_sols;
			fitness = m_fitness;
			belong = m_belong;
			dis2parent = m_dis2parent;
			popIters = m_popIter;
			popSolIds = m_popSolId;
			algIds = m_algId;
		}

	};

}

#endif 