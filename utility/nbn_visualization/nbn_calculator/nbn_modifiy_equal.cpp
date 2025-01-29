#include "nbn_modifiy_equal.h"
#include <vector>
#include <thread>
#include <mutex>

#include "../nbn_fla/nbn_info.h"
#include "../nbn_fla/nbn_fla_utility.h"

namespace ofec {

	namespace nbn {
		// update
		void compareIdx_modifyEqualSolutionsTask(
			int curId,
			int comId,
			NBNinfo& info,
			const std::vector<int>& sortedIds,
			ofec::Environment* env) {

			int curSolId = sortedIds[curId];
			int parentSolId = sortedIds[comId];

			double curdis = info.m_solbases[curSolId]->variableDistance(info.m_solbases[parentSolId]->variableBase(), env);
			//double curdis = m_distanceFun(curSolId, parentSolId);
			auto& distance = info.m_dis2parent;
			auto& belong = info.m_belong;

			if (distance[curSolId] > curdis) {
				distance[curSolId] = curdis;
				belong[curSolId] = parentSolId;
			}
			//else if (distance[curSolId] == curdis && rnd->uniform.next() < 0.5) {
			//	distance[curSolId] = curdis;
			//	belong[curSolId] = parentSolId;
			//}

		}


		void modifyEqualSolutionsTask(
			int curId,
			NBNinfo& info,
			const std::vector<int>& sortedIds,
			const std::vector<std::pair<int, int>>& fitPairIds,
			const std::vector<int>& id2FitPairIds,
			ofec::Environment* env,
			ofec::Random* rnd
		) {
			const int m_equalSize = 100;


			auto& curFitPairId = fitPairIds[id2FitPairIds[curId]];
			if (curFitPairId.second - curId > m_equalSize) {
				int loop = m_equalSize;
				while (loop--) {
					int otherId = rnd->uniform.nextNonStd<int>(curId + 1, curFitPairId.second + 1);
					compareIdx_modifyEqualSolutionsTask(curId, otherId, info, sortedIds, env);
				}
			}
			else {
				for (int idx(curId + 1); idx < curFitPairId.second; ++idx) {
					compareIdx_modifyEqualSolutionsTask(curId, idx, info, sortedIds, env);
				}
			}


		}




		void modifyEqualSolutionsThreadTask(
			std::mutex& mtx,
			int& maxId,
			NBNinfo& info,
			const std::vector<int>& sortedIds,
			const std::vector<std::pair<int, int>>& fitPairIds,
			const std::vector<int>& id2FitPairIds,
			ofec::Environment* env,
			ofec::Random* rnd) {
			auto& vFitness = info.m_vFitness;
			int curId = -1;
			while (true) {
				curId = -1;
				{
					mtx.lock();
					curId = maxId++;
					mtx.unlock();
				}
				if (curId >= vFitness.size())break;
				modifyEqualSolutionsTask(curId, info, sortedIds, fitPairIds, id2FitPairIds, env, rnd);
			}

		}



		void modifyEqualSolutions(
			NBNinfo& info,
			ofec::Environment* env,
			ofec::Random* rnd) {

			if (info.m_vFitness.empty())return;
			std::vector<std::pair<int, int>> fitPairIds;
			std::pair<int, int> pairIds = { 0,0 };
			pairIds.first = pairIds.second = 0;

			auto& vFitness = info.m_vFitness;

			
			std::vector<int> sortedIds;
			
		
			
			{
				std::vector<int> treeIdx(info.m_solbases.size(),-1);
				int rootId(0);
				for (int idx(0); idx < info.m_belong.size(); ++idx) {
					if (info.m_belong[idx] == idx) {
						rootId = idx;
						break;
					}
				}
				std::vector<std::vector<int>> directBasin;
				
				ofec::nbn::getDirectBasin(info, directBasin);
				
				std::queue<int> que;
				que.push(rootId);
				int curTreeId(0);
				while (!que.empty()) {
					int cur = que.front();
					que.pop();
					treeIdx[cur] = curTreeId++;
					for (auto& it : directBasin[cur]) {
						que.push(it);
					}
					
				}
				

				sortedIds.resize(vFitness.size());
				for (int idx(0); idx < sortedIds.size(); ++idx) {
					sortedIds[idx] = idx;

				}
				std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
					if (vFitness[a] < vFitness[b]) {
						return true;
					}
					else if (vFitness[a] == vFitness[b] ) {
						return treeIdx[a] > treeIdx[b];
						//return true;
					}
					else return false;
					});
			//	sortedIds.pop_back();

			}


			for (int idx(1); idx < sortedIds.size(); ++idx) {
				if (vFitness[sortedIds[pairIds.first]] == vFitness[sortedIds[idx]]) {
					pairIds.second = idx;
				}
				else {
					fitPairIds.push_back(pairIds);
					pairIds.first = pairIds.second = idx;
				}
			}
			fitPairIds.push_back(pairIds);


			std::vector<int> id2FitPairIds(vFitness.size(), -1);
			int curId = 0;
			for (int idx(0); idx < id2FitPairIds.size(); ++idx) {
				if (vFitness[sortedIds[idx]] != vFitness[sortedIds[fitPairIds[curId].first]]) {
					++curId;
				}
				id2FitPairIds[idx] = curId;
			}


			{
				std::mutex calMtx;
				int maxId = 0;

				int numThread = std::thread::hardware_concurrency();

				std::vector<std::shared_ptr<ofec::Random>> rnds(numThread);
				for (auto& it : rnds) {
					it.reset(new ofec::Random(rnd->uniform.next()));
				}
				
				std::vector<std::thread> thrds;
				for (size_t i = 0; i < numThread; ++i) {
					thrds.push_back(std::thread(
						&modifyEqualSolutionsThreadTask, 
						std::ref(calMtx),
						std::ref(maxId),
						std::ref(info),
						std::cref(sortedIds),
						std::cref(fitPairIds), std::cref(id2FitPairIds),env, rnds[i].get()));
				}
				for (auto& thrd : thrds)
					thrd.join();
			}




		}

	
	}

}