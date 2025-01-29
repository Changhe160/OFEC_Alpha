#ifndef NBN_NEAREST_BETTER_CALCULATOR_H
#define NBN_NEAREST_BETTER_CALCULATOR_H

#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <algorithm>
#include <shared_mutex>
#include <chrono>


#include "../../../core/problem/solution.h"
#include "../../../core/problem/problem.h"
#include "../../../core/environment/environment.h"
#include "../../../utility/function/custom_function.h"
#include "../../../utility/function/visit_lazy_list.h"

namespace ofec {
	struct NBN_NearestBetterCalculator {

	protected:

		struct NodeValueStruct {
			int m_cur_id = 0;
			double value = 0;
			bool operator<(const NodeValueStruct& a) const
			{
				return compareLarger(m_cur_id, value, a.m_cur_id, a.value);
				//return value > a.value;
			}
			NodeValueStruct() = default;
			NodeValueStruct(int id, double curvalue) :m_cur_id(id), value(curvalue) {}
			void setId(int id) {
				m_cur_id = id;
			}

		};

	

	protected:



		static void resetVisited(std::vector<unsigned long long>& visited,
			unsigned long long& curVisited) {
			curVisited = 0;
			std::fill(visited.begin(), visited.end(), curVisited);
		}




	public:




		static bool compareLarger(int curIdA, double value1, int curIdB, double value2) {
			if (value1 == value2) {
				return curIdA > curIdB;
			}
			else return value1 > value2;
		}


		static void calculate(const std::vector<SolutionBase*>& m_sols,
			const std::vector<double>& fitness,
			const std::vector<std::vector<int>>& neighbors,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			ofec::Problem* pro, ofec::Random* rnd) {
			std::vector<unsigned long long> visited(m_sols.size(), 0);
			unsigned long long cur_visited = 0;
			std::vector<std::vector<int>> better_range_nodeId(m_sols.size());
			belong.resize(m_sols.size());
			std::fill(belong.begin(), belong.end(), -1);

			dis2parent.resize(m_sols.size());
			std::fill(dis2parent.begin(), dis2parent.end(), std::numeric_limits<double>::max());

			std::vector<int> sorted_idx(m_sols.size());
			for (int idx(0); idx < sorted_idx.size(); ++idx) {
				sorted_idx[idx] = idx;
			}
			//	auto& nbn_nodes(curTreeNode.m_division_nodes);
			std::sort(sorted_idx.begin(), sorted_idx.end(), [&](
				int a, int  b
				) {

	/*				if (fitness[a] < fitness[b]) {
						return true;
					}
					else if (fitness[a] == fitness[b]) {
						return pro->compareSolutionsSmaller(m_sols[a]->variableBase(), a, m_sols[b]->variableBase(), b);
					}
					else return false;*/


				return fitness[a] < fitness[b];

			});

			


			//curTreeNode.m_cur_visited2 = 0;
			//for (auto& curNode : curTreeNode.m_division_nodes) {
			//	curNode.m_better_range_nodeId.clear();
			//	curNode.m_better_range_nodeId.push_back(curNode.m_node_id);
			//}
			for (int sidx(0); sidx + 1 < sorted_idx.size(); ++sidx) {
				//std::cout << "curId\t" << sidx << std::endl;
				auto sortedId = sorted_idx[sidx];
				//	auto& cur_node(nbn_nodes[sortedId]);

				updateParent(m_sols, fitness, neighbors, sortedId,
					better_range_nodeId, belong, dis2parent, visited, cur_visited,
					pro, rnd);

			}

			belong[sorted_idx.back()] = sorted_idx.back();

			//	curTreeNode.m_best_divNodeId = sorted_idx.back();
			//	curTreeNode.m_best_solId = nbn_nodes[sorted_idx.back()].m_representative_solId;

			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] == -1) belong[idx] = idx;
			}
		}





		static void updateParent(const std::vector<SolutionBase*>& m_sols,
			const std::vector<double>& fitness,
			const std::vector<std::vector<int>>& neighbors,
			int curSolId,
			std::vector<std::vector<int>>& better_range_nodeId,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			std::vector<unsigned long long>& visited,
			unsigned long long& curVisited,
			ofec::Problem* pro, ofec::Random* rnd) {
			if (++curVisited == 0) {
				resetVisited(visited, curVisited);
			}
			visited[curSolId] = curVisited;

			auto& cur_sol(m_sols[curSolId]);

			std::priority_queue<NodeValueStruct> nei_que;
			NodeValueStruct curQueNode;
			auto curNeibor = neighbors[curSolId];

			rnd->uniform.shuffle(curNeibor.begin(), curNeibor.end());
			for (auto& nei_info : curNeibor) {
				auto& nei_idx = nei_info;
				//auto nei_node(&nbn_nodes[nei_idx]);
				if (visited[nei_idx] != curVisited) {
					visited[nei_idx] = curVisited;
					//curQueNode.m_cur = nei_node;
					curQueNode.m_cur_id = nei_idx;
					{
						auto& nei_sol(m_sols[nei_idx]);
						curQueNode.value = pro->variableDistance(cur_sol->variableBase(),nei_sol->variableBase());
						//cur_node.m_dis2parent = cur_sol->norVariableDistance(*par_sol, m_id_pro);
					}
					nei_que.push(curQueNode);
				}
			}


			std::vector<int> attraction;
			while (!nei_que.empty()) {
				curQueNode = nei_que.top();
				auto nei_node = curQueNode.m_cur_id;

				if (belong[nei_node] != -1)
					//if (m_belong[nei_node->m_belong] != nei_node->m_representative_solId)
					/*if (m_fitness[cur_node.m_representative_solId] > m_fitness[nei_node->m_representative_solId])*/ {
					nei_que.pop();
					attraction.push_back(curQueNode.m_cur_id);
					for (auto& son_idx : better_range_nodeId[nei_node]) {
						//	auto son_node(&nbn_nodes[son_idx]);
						if (visited[son_idx] != curVisited) {
							visited[son_idx] = curVisited;
							//curQueNode.m_cur = son_node;
							curQueNode.m_cur_id = son_idx;
							{

								auto& son_sol(m_sols[son_idx]);
								curQueNode.value = pro->variableDistance(cur_sol->variableBase(), son_sol->variableBase());
								//cur_node.m_dis2parent = cur_sol->norVariableDistance(*par_sol, m_id_pro);
							}
							nei_que.push(curQueNode);
						}
					}
				}
				else break;
			}
			if (!nei_que.empty()) {
				//cur_node.m_better_range_nodeId = peak_ranges;
				better_range_nodeId[curSolId].clear();
				NodeValueStruct parentNode = nei_que.top();
				while (!nei_que.empty()) {
					curQueNode = nei_que.top();
					nei_que.pop();
					better_range_nodeId[curSolId].push_back(curQueNode.m_cur_id);
				}


				//	cur_node.m_direct_parent_nodeId = cur_node.m_parent_nodeId = parentNode.m_cur_id;
				if (parentNode.m_cur_id != -1) {
					//	int curSolId = cur_node.m_representative_solId
					int parSolId = parentNode.m_cur_id;
					dis2parent[curSolId] = parentNode.value;
					belong[curSolId] = parSolId;

				}
			}
		}



		static void modifySols(const std::vector<SolutionBase*>& m_sols,
			const std::vector<double>& fitness,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			ofec::Problem* pro, ofec::Random* rnd) {

			std::vector<int> solIds(m_sols.size());
			for (int idx(0); idx < solIds.size(); ++idx) {
				solIds[idx] = idx;
			}
			std::sort(solIds.begin(), solIds.end(), [&](int a, int b) {
				return compareLarger(a, fitness[a], b, fitness[b]);
				//return fitness[a] < fitness[b];
			});


			for (int idx(0); idx < solIds.size(); ++idx) {
				auto curSolId = solIds[idx];
				auto& cursol = m_sols[curSolId];
				if (belong[idx] == idx) {
					for (int idy(idx + 1); idy < solIds.size(); ++idy) {
						auto otherSolId = solIds[idy];
						auto& otherSol = m_sols[otherSolId];
						double dis = pro->variableDistance(cursol->variableBase(), otherSol->variableBase());
						if (dis < dis2parent[curSolId]) {
							dis2parent[curSolId] = dis;
							belong[curSolId] = otherSolId;
						}
					}
				}
			}
		}
	};

}


#endif