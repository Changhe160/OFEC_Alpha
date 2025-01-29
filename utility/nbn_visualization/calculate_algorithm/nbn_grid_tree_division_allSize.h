

#ifndef NBN_GRID_DIVISION_TREE_ALLSIZE_H
#define NBN_GRID_DIVISION_TREE_ALLSIZE_H

#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>


#include "../../../core/problem/solution.h"
#include "../../../utility/function/custom_function.h"
#include "nbn_division_base.h"


//#include "core/problem/solution.h"
//#include "utility/typevar/typevar.h"



namespace ofec {
	class NBN_GridTreeDivision_allSize : public NBN_DivisionBase {
	public:
		using SolutionType = SolutionBase;
		using SolContinousType = Solution<>;
		struct Node {



			int m_node_id = -1;
			int m_representative_solId = -1;
			std::shared_ptr<SolutionType> m_representative_sol = nullptr;
			int m_belong = -1;
			//double m_dis2parent = 0;
			std::vector<int> m_vec;
			//	unsigned long long m_visited_stamp = 0;
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
				m_representative_sol = nullptr;
				//m_parent_solId = -1;
			//	m_dis2parent = 0;
				m_vec.clear();
				m_visited_stamp2 = 0;
				m_neighbor_nodeIds.clear();
				//	m_neighbor_nodeId_dis.clear();
				m_parent_nodeId = -1;
				m_direct_parent_nodeId = -1;
				m_better_range_nodeId.clear();
				m_sample_solIds.clear();
			}
			void initialize(int id) {
				clear();
				m_node_id = id;



				//	m_dis2parent = std::numeric_limits<double>::max();
			}
		};

		struct TreeNode {


			int m_depth = -1;
			
			int m_son_nodeId = -1;
			//			std::shared_ptr<TreeNode> m_parent = nullptr;

			int m_best_solId = -1;
			int m_best_divNodeId = -1;

			int m_numberDivision = 0;
			//	unsigned long long m_cur_visited = 0;
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


			//int m_updateNode = 0;
			//std::mutex m_updateMtx;



			void resizeNumberDivision(int numDiv);

			//void resetVisited() {
			//	m_cur_visited = 0;
			//	for (auto& it : m_division_nodes) it.m_visited_stamp = m_cur_visited;
			//}
			void resetVisited2() {
				m_cur_visited2 = 0;
				for (auto& it : m_division_nodes) it.m_visited_stamp2 = m_cur_visited2;
			}

			void clear() {
				m_neighbors.clear();
				m_division_nodes.clear();
				m_trees.clear();
			}

			void initialize(const std::vector<std::pair<double, double>>& boundary,
				int sampleSize,const std::vector<int>&user_div);
			void getSubSpace(int nodeId, std::vector<std::pair<double, double>>& boundary);
			//	void mergeParent(Node* curNode);
			//	void findParent(Node* curNode, Node*& parent);


		};





	protected:

		std::shared_ptr<TreeNode> m_root;
		//// updateNodeInfo
		//std::vector<int> m_node_paths;
		//bool m_flag_node_division = false;
		std::vector<int> m_user_div ;
		int m_division_threadhold = 1000;
		int m_min_div = 100;

		double m_worstFit = -std::numeric_limits<double>::max();

		//std::vector<std::shared_ptr<SolutionType>> m_added_sols;
		std::vector<std::shared_ptr<SolutionType>> m_sols;
		std::vector<double> m_fitness;
		std::vector<int> m_belong;
		std::vector<double> m_dis2parent;


		int m_solsFitnessUpdatedIdxs = 0;




		// nbn info
	protected:


		void updateNeighborInfo(int idx, int dim, std::vector<Node>& division_nodes, const std::vector<int>& m_dim_div) {
			std::vector<int> neiVec;
			int neiIdx(0);

			std::vector<int> curVec(dim, 0);
			int curIdx(0);
			/*for (int idx(0); idx < m_numberDivision; ++idx) */ {
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
		bool judgeDivisionFlag(int sampleSize) {
			return sampleSize >= m_division_threadhold /*|| (double(sampleSize) > pow(2, m_dim) && sampleSize <= m_min_div)*/;
		}

		void updateFitnessThreadTask(int from, int to);
		void assignedSols(int numSols);
		//void assignedSol();
		//void divideSpace(std::shared_ptr<TreeNode>&, const std::vector<std::pair<double, double>>& boundary, int sampleSize);
		//void udpateSubspace(std::shared_ptr<TreeNode>&, int curIdx);
		//void divideSubSpace(std::shared_ptr<TreeNode>&, int curIdx);
		//void generateSolution(TreeNode& curNode);
		//void calculateNetwork(TreeNode& curNode);

		void updateNeighborAndParent(TreeNode& curTreeNode, Node& cur_node) {
			if (++curTreeNode.m_cur_visited2 == 0) {
				curTreeNode.resetVisited2();
				++curTreeNode.m_cur_visited2;
			}
			auto& nbn_nodes(curTreeNode.m_division_nodes);
			cur_node.m_visited_stamp2 = curTreeNode.m_cur_visited2;

			auto& cur_sol(cur_node.m_representative_sol);

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
						auto& nei_sol(nei_node->m_representative_sol);
						curQueNode.m_cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), nei_sol->variableBase());
						//cur_node.m_dis2parent = cur_sol->normalizedVariableDistance(*par_sol, m_id_pro);
					}
					nei_que.push(curQueNode);
				}
			}


			std::vector<Node*> attraction;
			while (!nei_que.empty()) {
				curQueNode = nei_que.top();
				auto nei_node = curQueNode.m_cur;
				if (nei_node->m_direct_parent_nodeId != -1)
					//if (m_belong[nei_node->m_belong] != nei_node->m_representative_solId)
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

								auto& son_sol(son_node->m_representative_sol);
								curQueNode.m_cur_dis = m_environment->problem()->normalizedVariableDistance(cur_sol->variableBase(), son_sol->variableBase());
								//cur_node.m_dis2parent = cur_sol->normalizedVariableDistance(*par_sol, m_id_pro);
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
				if (cur_node.m_representative_solId != -1) {
					int curSolId = cur_node.m_representative_solId;
					int parSolId = nbn_nodes[cur_node.m_direct_parent_nodeId].m_representative_solId;
					m_dis2parent[curSolId] = parentNode.m_cur_dis;
					m_belong[curSolId] = parSolId;

				}
			}
		}


		void calculateNetworkAccurate(TreeNode& curNode);
		void calSolsNetworkThreadTask(int from, int to, std::vector<int>& nodeId, const std::vector<int>& solIdxs) {}
		void divideSolsNetwork(TreeNode& curNode, const std::vector<int>& solIdxs);
		void updateNetwork(
			TreeNode& curNode,
			const std::vector<int>& solIdxs,
			const std::vector<std::pair<double, double>>& boundary);



		int  updateLocalNetwork(const std::vector<int>& solIdxs);
		void getSolId(const SolutionBase& new_sol, std::vector<int>& nodeIds);




		virtual void addSol(const SolutionBase& new_sol, int& belong_id, bool flag_opt = false,
			int popIter = -1, int popSolId = -1, int algId = -1) override {
			//		insertSol(new_sol, belong_id, flag_opt, popIter, popSolId, algId);
		}

		void setSol(int solId, std::shared_ptr<SolutionBase>& sol) {
			m_fitness[solId] = sol->fitness();
			m_sols[solId] = sol;
		}
	public:


		std::vector<int>& getUserDiv() {
			return m_user_div;
		}


		virtual void initialize_(bool flag_grid_sample = true) override {
			if (m_root != nullptr) {
				m_root->m_trees.clear();
			}

			m_root = nullptr;
		}


		static void vecToIdx(const std::vector<int>& cur, int& idx, const std::vector<int>& dim_div);
		static void solToVec(const SolutionBase& sol, std::vector<int>& vec, const std::vector<int>& dim_div,
			const std::vector<double>& range_div,
			const std::vector<double>& range_from);

		static void vecToCenterSol(SolutionBase& sol, const std::vector<int>& vec, const std::vector<int>& dim_div,
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


		NBN_GridTreeDivision_allSize() = default;
		~NBN_GridTreeDivision_allSize() {
			if (m_root != nullptr) {
				m_root->m_trees.clear();
			}
			m_root = nullptr;
		}
		virtual size_t size()  const  override {
			return m_sols.size();
		}

		//void updateGlobalNetwork();

		virtual void generateSols();
		virtual void updateFitness();
		virtual void addSol(const SolutionBase& new_sol) {
			std::shared_ptr<SolutionBase> cur_sol(m_environment->problem()->createSolution(new_sol));
			assignedSols(1);
			m_sols.back() = cur_sol;
		}

		virtual int inputSol(const SolutionBase& new_sol) {
			std::shared_ptr<SolutionBase> cur_sol(m_environment->problem()->createSolution(new_sol));
			int solId = m_sols.size();
			assignedSols(1);
			m_sols.back() = cur_sol;
			return solId;
		}


		
		//virtual void addRandom(int numSample) {
		//	using namespace ofec;
		//	//SolContinousType sol(m_pro->numObjectives(),
		//	//	m_pro->numConstraints(),
		//	//	m_pro->numVariables());

		//	std::unique_ptr<ofec::SolutionBase> sol;
		//	for (int idx(0); idx < numSample; ++idx) {
		//		sol.reset(m_pro->createSolution());
		//		sol.initialize(m_pro.get(), m_random.get());
		//		int belong_id(0);
		//		addSol(sol, belong_id);
		//	}
		//}
		int getSolNodeId(const SolutionBase& sol) {
			int nodeId(-1);
			getSolInNodeId(sol, *m_root, nodeId);
			return nodeId;
		}

		//void updateDivision();
		//void updateDivision(std::shared_ptr<TreeNode>&, const std::vector<std::pair<double, double>>& boundary, int sampleSize);
		//

		void generateNetwork();
		

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
		//	popIters = m_popIter;
		//	popSolIds = m_popSolId;
	//		algIds = m_algId;
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
			flagOpt.resize(m_sols.size());
			std::fill(flagOpt.begin(), flagOpt.end(), false);
		//	flagOpt = m_flagOpt;
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
		}

	};

}

#endif 