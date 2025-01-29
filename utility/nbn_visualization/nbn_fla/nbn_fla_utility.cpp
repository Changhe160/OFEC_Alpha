#include "nbn_fla_utility.h"

#include <vector>
#include <queue>
#include <ostream>
#include <set>


#include "../../../core/random/newran.h"
#include "../../../core/problem/solution.h"
#include "../../../core/problem/solution.h"
#include "../../../core/problem/continuous/continuous.h"
#include "../../../core/problem/problem.h"
#include "../../linear_algebra/vector.h"
#include "../../functional.h"
#include "../../function/custom_function.h"
namespace ofec {
	namespace nbn {
		double analyzeFitnesslandscapeDifference(
			const std::vector<int>& belong,
			const std::vector<double>& fitness) {
			int numSame(0);

			for (int idx(0); idx < fitness.size(); ++idx) {
				if (fitness[idx] <= fitness[belong[idx]]) {
					++numSame;
				}
			}
			return double(numSame) / fitness.size();
		}


		bool judgeNBN(
			const std::vector<int>& belong,
			const std::vector<double>& fitness) {
			for (int idx(0); idx < belong.size(); ++idx) {
				if (fitness[idx] > fitness[belong[idx]]) {
					return false;
				}
			}
			return true;
		}


		//void calBasinSize(std::vector<int>& belong,
		//	std::vector<double>& basin) {

		//	std::vector<std::vector<int>> sons(belong.size());
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//		if (belong[idx] != idx) {
		//			sons[belong[idx]].push_back(idx);
		//		}
		//	}


		//	std::queue<int> que_nodes;
		//	std::vector<int> visited(belong.size(), 0);
		//	for (int idx(0); idx < sons.size(); ++idx) {
		//		visited[idx] = sons[idx].size();
		//		if (visited[idx] == 0) {
		//			que_nodes.push(idx);
		//		}
		//	}
		//	basin_size.resize(belong.size());
		//	std::fill(basin_size.begin(), basin_size.end(), 1);

		//	while (!que_nodes.empty()) {
		//		int cur = que_nodes.front();
		//		que_nodes.pop();

		//		for (auto& sonId : sons[cur]) {
		//			basin_size[cur] += (basin_size[sonId]);
		//		}

		//		if (--visited[belong[cur]] == 0) {
		//			que_nodes.push(belong[cur]);
		//		}
		//	}
		//}

		void calBasins(const std::vector<int>& belong,
			const std::vector<double>& dis2parent,
			double m_basin_radius_cut,
			std::vector<int>& optIds,
			std::vector<int>& belongBid,
			std::vector<std::vector<int>>& basin_idxs) {
			std::vector<std::vector<int>> sons(belong.size());
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] != idx) {
					sons[belong[idx]].push_back(idx);
				}
			}
			optIds.clear();
			for (int idx(0); idx < belong.size(); ++idx) {
				if (dis2parent[idx] > m_basin_radius_cut) {
					optIds.push_back(idx);
				}
			}

			{
				std::vector<int> visited(belong.size(), false);
				basin_idxs.resize(optIds.size());
				//std::vector<int> belongBid(belong.size(), -1);;

				belongBid.resize(belong.size(), -1);;
				std::fill(belongBid.begin(), belongBid.end(), -1);

				for (auto& it : optIds) {
					visited[it] = true;

				}
				std::queue<int> queInt;

				for (int idx(0); idx < optIds.size(); ++idx) {
					//	basin_idxs[idx].push_back(optIds[idx]);
					queInt.push(optIds[idx]);
					belongBid[optIds[idx]] = idx;
				}

				while (!queInt.empty()) {
					int cur = queInt.front();
					queInt.pop();
					for (auto& sonId : sons[cur]) {
						if (!visited[sonId]) {
							queInt.push(sonId);
							belongBid[sonId] = belongBid[cur];
						}
					}
				}
				for (int idx(0); idx < belong.size(); ++idx) {
					basin_idxs[belongBid[idx]].push_back(idx);
				}


				// for test 

				std::cout << "begin test" << std::endl;

				int stop = -1;


				for (int idx(0); idx < belong.size(); ++idx) {
					if (belongBid[idx] != belongBid[belong[idx]] && !visited[idx]) {
						int stop = -1;
						std::cout << idx << std::endl;
					}
				}

				std::cout << std::endl << std::endl;

			}
		}


		void getBasin(const std::vector<int>& belong,
			int peakId, std::vector<int>& curBasin) {
			//std::vector<std::vector<int>> sons;
			curBasin.clear();
			std::vector<std::vector<int>> sons(belong.size());
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] != idx) {
					sons[belong[idx]].push_back(idx);
				}
			}

			std::queue<int> que;
			que.push(peakId);
			while (!que.empty()) {
				int cur = que.front();
				que.pop();
				curBasin.push_back(cur);
				for (auto& it : sons[cur]) {
					que.push(it);
				}
			}
		}



		void getSubBasin(const std::vector<std::vector<int>>& sons,
			int peakId, std::vector<int>& curBasin) {
			curBasin.clear();
			std::queue<int> que;
			que.push(peakId);
			while (!que.empty()) {
				int cur = que.front();
				que.pop();
				curBasin.push_back(cur);
				for (auto& it : sons[cur]) {
					que.push(it);
				}
			}
		}


		void getSubBasin(const std::vector<std::set<int>>& sons,
			int peakId, std::vector<int>& curBasin) {
			curBasin.clear();
			std::queue<int> que;
			que.push(peakId);
			while (!que.empty()) {
				int cur = que.front();
				que.pop();
				curBasin.push_back(cur);
				for (auto& it : sons[cur]) {
					que.push(it);
				}
			}
		}


		void getDirectBasin(
			const NBNinfo& nbnInfo,
			std::vector<std::vector<int>>& basin_idxs
		) {
			auto& belong = nbnInfo.m_belong;
			basin_idxs.resize(belong.size());
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] != -1&&belong[idx]!=idx)
					basin_idxs[belong[idx]].push_back(idx);
			}
		}

		void getBasinSize(
			const NBNinfo& nbnInfo,
			std::vector<std::vector<int>>& sons,
			std::vector<int>& basinSizes) {

			auto& belong = nbnInfo.m_belong;


			std::vector<int> visited(sons.size(), 0);
			for (int idx(0); idx < visited.size(); ++idx) {
				visited[idx] = sons[idx].size();
			}
			basinSizes.resize(sons.size());
			std::fill(basinSizes.begin(), basinSizes.end(), 1);
			std::queue<int> queLeaves;
			for (int idx(0); idx < basinSizes.size(); ++idx) {
				if (sons[idx].empty()) {
					queLeaves.push(idx);
				}
			}

			while (!queLeaves.empty()) {
				int cur = queLeaves.front();
				queLeaves.pop();
				if (belong[cur] != -1) {
					basinSizes[belong[cur]] += basinSizes[cur];
					if (--visited[belong[cur]] == 0) {
						queLeaves.push(belong[cur]);
					}
				}
			}

		}


		void getBasinTreeDistanceRatio(
			const NBNinfo& nbnInfo,
			std::vector<Real>& distanceRatio
		) {

			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;
			distanceRatio.resize(belong.size());
			std::vector<std::vector<int>> basin_idxs;
			getDirectBasin(nbnInfo, basin_idxs);

			std::vector<Real> distance(dis2parent);
			double maxDis;
			for (auto& it : distance) {
				if (it < 1e9) {
					maxDis = std::max(maxDis, it);
				}
			}
			maxDis *= 1.1;
			for (auto& it : distance) {
				if (it > 1e9) it = maxDis;
			}

			for (int idx(0); idx < distance.size(); ++idx) {
				if (distance[idx] == 0) {
					distance[idx] = distance[belong[idx]];
					//std::cout << "distacne 0 \t" << idx << "\tbelong\t" << belong[idx] << std::endl;
				}
			}

			std::vector<Real> sonDistances;
			std::vector<Real> disRatio;


			std::vector<Real> avgSonDistances;

			for (int idx(0); idx < belong.size(); ++idx) {
				if (basin_idxs[idx].empty())continue;
				double avgSonDis = 0;
				sonDistances.clear();
				for (auto& it : basin_idxs[idx]) {
					sonDistances.push_back(distance[it]);
				}
				calMean(sonDistances, avgSonDis);


				distanceRatio[idx] = distance[idx] / avgSonDis;
				avgSonDistances.push_back(avgSonDis);

				disRatio.push_back(distanceRatio[idx]);
			}

			double minDisRatio(0);
			calMin<double, double>(disRatio, minDisRatio);
			minDisRatio *= 0.9;
			for (int idx(0); idx < belong.size(); ++idx) {
				if (basin_idxs[idx].empty()) {
					distanceRatio[idx] = minDisRatio;
				}
			}

			//{
			//	double maxDistanceRatio(0);
			//	double minDistanceRatio(0);
			//	calMax(distanceRatio, maxDistanceRatio);
			//	calMin(distanceRatio, minDistanceRatio);

			////	std::cout << "max Distnace ratio\t" << maxDistanceRatio << std::endl;
			////	std::cout << "min distance ratio\t" << minDistanceRatio << std::endl;
			//}


			//{
			//	double minValue(0), maxValue(0);
			//	calMax(avgSonDistances, maxValue);
			//	calMin(avgSonDistances, minValue);
			////	std::cout << "max avgSonDistances \t" << maxValue << std::endl;
			////	std::cout << "min avgSonDistances \t" << minValue << std::endl;
			//}


			//{
			//	double minValue(0), maxValue(0);
			//	calMax(distance, maxValue);
			//	calMin(distance, minValue);
			////	std::cout << "max distance \t" << maxValue << std::endl;
			////	std::cout << "min distance \t" << minValue << std::endl;
			//}

		}


		void calculateBasins(
			std::vector<std::vector<int>>& basins,
			const NBNinfo& nbnInfo,
			const std::vector<std::vector<int>>& selectedCenters
		) {

			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;

			std::vector<std::vector<int>> direct_sons;
			getDirectBasin(nbnInfo, direct_sons);
			std::vector<int> belongBasinId(belong.size(), -1);
			basins.resize(selectedCenters.size());


			std::queue<int> queueIds;
			for (int idx(0); idx < selectedCenters.size(); ++idx) {
				for (auto& it : selectedCenters[idx]) {
					if (it < belong.size()) {
						if (belongBasinId[it] == -1) {
							belongBasinId[it] = idx;
							queueIds.push(it);
							basins[idx].push_back(it);
						}
					}
				}
			}

			while (!queueIds.empty()) {
				int cur = queueIds.front();
				int idx = belongBasinId[cur];
				queueIds.pop();
				for (auto& it : direct_sons[cur]) {
					if (belongBasinId[it] == -1) {
						belongBasinId[it] = idx;
						queueIds.push(it);
						basins[idx].push_back(it);
					}
				}
			}


		}



		void calculateBasins(
			std::vector<std::vector<int>>& basins,
			const NBNinfo& nbnInfo,
			const std::vector<int>& selectedCenter
		) {


			

			std::vector<std::vector<int>> selectedCenters(selectedCenter.size());
			for (int idx(0); idx < selectedCenter.size(); ++idx) {
				selectedCenters[idx].push_back(selectedCenter[idx]);
			}

			calculateBasins(basins, nbnInfo,selectedCenters );
		}

		void calculateBasinBestIds(
			const NBNinfo& nbnInfo,
			const std::vector<std::vector<int>>& basins,
			std::vector<int>& basinBestIds
		)
		{
			auto& fitness = nbnInfo.m_vFitness;
			basinBestIds.clear();
			//basinBestIds.resize(basins.size());

			for (int idx(0); idx < basinBestIds.size(); ++idx) {
				Real bestFit = std::numeric_limits<Real>::lowest();
				std::vector<int> bestIds;
				for (auto& it : basins[idx]) {
					if (bestFit < fitness[idx]) {
						bestFit = fitness[idx];
						bestIds.clear();
						bestIds.push_back(idx);
					}
					else if (bestFit == fitness[idx]) {
						bestIds.push_back(idx);
					}

				}

				for (auto& it : bestIds) {
					basinBestIds.push_back(it);
				}
			}
		}


		void divideBasinByDistanceBasinSize(
			const NBNinfo& nbnInfo,
			double distanceThreadhold,
			int basinSizeThreadhold,
			std::vector<std::vector<int>>& basins) {

			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;

			std::vector<int> basinSize;
			std::vector<std::vector<int>> sons;

			getDirectBasin(nbnInfo, sons);
			getBasinSize(nbnInfo, sons, basinSize);

			basins.clear();
			std::vector<int> sortedIds;


			int rootId = -1;


			for (int idx(0); idx < belong.size(); ++idx)
			{
				if (belong[idx] != -1)
					sortedIds.push_back(idx);
				else rootId = idx;
			}
			std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
				return dis2parent[a] > dis2parent[b];
				});

			std::vector<bool> flagFeasible(belong.size());
			std::fill(flagFeasible.begin(), flagFeasible.end(), true);


			std::vector<std::set<int>>  set_sons(sons.size());
			for (int idx(0); idx < sons.size(); ++idx) {
				for (auto& it : sons[idx]) {
					set_sons[idx].insert(it);
				}
			}


			for (auto& it : sortedIds) {
				if (flagFeasible[it]
					&& dis2parent[it] >= distanceThreadhold
					&& basinSize[it] >= basinSizeThreadhold) {


					basins.push_back(std::vector<int>());
					getSubBasin(set_sons, it, basins.back());
					for (auto& it2 : basins.back()) {
						flagFeasible[it2] = false;
					}
					if (belong[it] != -1) {
						set_sons[belong[it]].erase(it);
					}

					int parentId = belong[it];
					while (parentId != -1) {
						basinSize[parentId] -= basinSize[it];
						parentId = belong[parentId];
					}
				}
			}

			basins.push_back(std::vector<int>());
			getSubBasin(set_sons, rootId, basins.back());





		}


		double calculateNBDstd(const NBNinfo& nbnInfo) {
			std::vector<double> dis2parent;
			double meanValue, stdValue(0);
			
			for (int idx(0); idx < nbnInfo.m_dis2parent.size(); ++idx) {
				if (nbnInfo.m_belong[idx] != idx && nbnInfo.m_belong[idx] != -1) {
					dis2parent.push_back(nbnInfo.m_dis2parent[idx]);
				}
			}

			calMean(dis2parent, meanValue);
			calStd(dis2parent, meanValue, stdValue);
			return stdValue;
		}

		double calculateNeutralityArea(
			const NBNinfo& nbnInfo,
			double ruggness_eps) {
			std::vector<int> neurtual_size(nbnInfo.m_dis2parent.size(), 1);

			std::vector<std::vector<int>> sons;
			getDirectBasin(nbnInfo, sons);
			std::queue<int> que_nodes;
			std::vector<int> visited(nbnInfo.m_dis2parent.size(), 0);
			for (int idx(0); idx < sons.size(); ++idx) {
				visited[idx] = sons[idx].size();
				if (visited[idx] == 0) {
					que_nodes.push(idx);
				}
			}



			auto norFit = nbnInfo.m_vFitness;
			auto& dis2parent = nbnInfo.m_dis2parent;
			ofec::dataNormalize(norFit);
			while (!que_nodes.empty()) {
				int cur = que_nodes.front();
				que_nodes.pop();

				for (auto& sonId : sons[cur]) {
					if (/*dis2parent[sonId] <= dis && */abs(norFit[cur] - norFit[sonId]) <= ruggness_eps * dis2parent[sonId]) {
						neurtual_size[cur] += neurtual_size[sonId];
					}
				}
				if (nbnInfo.m_belong[cur] != -1 && nbnInfo.m_belong[cur] != cur) {
					if (--visited[nbnInfo.m_belong[cur]] == 0) {
						que_nodes.push(nbnInfo.m_belong[cur]);
					}
				}

	
			//	que_nodes.push(nbnInfo.m_belong[cur]);
			}


			double maxNeutural = 0;
			for (int idx(0); idx < nbnInfo.m_dis2parent.size(); ++idx) {
				maxNeutural = std::max<double>(maxNeutural, double(neurtual_size[idx]) / nbnInfo.m_dis2parent.size());;
			}

			return maxNeutural;
		}
		int calculateMaxNeutralBasins(
			std::vector<int>& neurtual_size,
			const NBNinfo& nbnInfo,
			double neuralityRatio) {

			auto& fitness = nbnInfo.m_vFitness;
			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;


			auto norFit = fitness;
			ofec::dataNormalize(norFit);

			std::vector<std::vector<int>> sons;
			getDirectBasin(nbnInfo, sons);


			int rootId(-1);
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] == -1) {
					rootId = idx;
					break;
				}
			}


			neurtual_size.resize(belong.size(), 1);
			std::queue<int> que_nodes;
			std::vector<int> visited(belong.size(), 0);
			for (int idx(0); idx < sons.size(); ++idx) {
				visited[idx] = sons[idx].size();
				if (visited[idx] == 0) {
					que_nodes.push(idx);
				}
			}

			while (!que_nodes.empty()) {
				int cur = que_nodes.front();
				que_nodes.pop();

				for (auto& sonId : sons[cur]) {
					if (/*dis2parent[sonId] <= dis && */abs(norFit[cur] - norFit[sonId]) <= neuralityRatio) {
						neurtual_size[cur] += neurtual_size[sonId];
					}
					if (--visited[belong[cur]] == 0) {
						que_nodes.push(belong[cur]);
					}
				}



			}

			int maxNeutrality(0);
			calMax(neurtual_size, maxNeutrality);
			return maxNeutrality;

		}


		void getOptimumIds(
			std::vector<int>& optIds,
			const NBNinfo& nbnInfo,
			double bestRatio,double distanceRatio) {
			
			auto& fitness = nbnInfo.m_vFitness;
			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;

			optIds.clear();
		
			auto norFit = fitness;
			ofec::dataNormalize(norFit);
			
			double fitRatio = (1.0 - bestRatio);
			int numBNodes(0);
			double totalNBD(0);
			for (int idx(0); idx < norFit.size(); ++idx) {
				if (norFit[idx] >= fitRatio && belong[idx]!=-1&&belong[idx]!=idx) {
					++numBNodes;
					totalNBD += dis2parent[idx];
				}
			}

			double meanDis = totalNBD / numBNodes * distanceRatio;

			for (int idx(0); idx < norFit.size(); ++idx) {

				//if (belong[idx] == idx) {
				//	int stop = -1;
				//}

				if ((norFit[idx] >= fitRatio&& dis2parent[idx]>= meanDis)|| belong[idx]==-1||belong[idx]==idx) {
					optIds.push_back(idx);
				}
			}
			
		}


		void getOptimumIdsByDistance(
			std::vector<int>& optIds,
			const NBNinfo& nbnInfo,
			double distanceRatio ) {
			
			optIds.clear();
			
			auto& fitness = nbnInfo.m_vFitness;
			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;

			for (int idx(0); idx < nbnInfo.m_belong.size(); ++idx) {
				if (( dis2parent[idx] >= distanceRatio) || belong[idx] == -1 || belong[idx] == idx) {
					optIds.push_back(idx);
				}
			}
		}

	

		double calculateMaxNBDofOptimum(
			const NBNinfo& nbnInfo,
			const std::vector<int>& optIds) {
			
			if (optIds.empty()) {
				return 0;
			}
			else {

				auto& belong = nbnInfo.m_belong;
				auto& dis2parent = nbnInfo.m_dis2parent;
				double maxDis(std::numeric_limits<double>::lowest());
				for (auto& it : optIds) {
					if (belong[it] != -1 && belong[it] != it) {
						maxDis = std::max(maxDis, dis2parent[it]);
					}
				}
				return maxDis;
			}

		}


		double basinSizesSTD(const std::vector<std::vector<int>>& basins) {
			std::vector<double> basinSizes(basins.size());
			for (int idx(0); idx < basinSizes.size(); ++idx) {
				basinSizes[idx] = basins[idx].size();
			}

			double meanV(0), stdV(0);
			calStd(basinSizes, meanV, stdV);
			return stdV;
		}



		double maxNeutralitySons(const NBNinfo& nbnInfo, double neuralityRatio ) {

			auto& fitness = nbnInfo.m_vFitness;
			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;
			auto norFit = fitness;
			ofec::dataNormalize(norFit);

			std::vector<std::vector<int>> sons;
			getDirectBasin(nbnInfo, sons);


			//int rootId(-1);
			//for (int idx(0); idx < belong.size(); ++idx) {
			//	if (belong[idx] == -1) {
			//		rootId = idx;
			//		break;
			//	}
			//}


			std::vector<int> neurtual_size;
			calculateMaxNeutralBasins(neurtual_size, nbnInfo, neuralityRatio);

			int neuSize(0);
			int maxSize(0);
			for (int idx(0); idx < belong.size(); ++idx) {
				if (!sons[idx].empty()) {
					auto& curSons = sons[idx];
					sort(curSons.begin(), curSons.end(), [&](int a, int b) {
						return fitness[a] > fitness[b];
						});

					neuSize = neurtual_size[curSons.front()];
					maxSize = std::max(maxSize, neuSize);
					for (int idy(1); idy < curSons.size(); ++idy) {

						if (abs(norFit[curSons[idy - 1]] - norFit[curSons[idy]]) <= neuralityRatio) {
							neuSize += neurtual_size[curSons[idy]];
							maxSize = std::max(maxSize, neuSize);
						}
						else {
							neuSize = 0;
						}
					}
				}
				
			}
			return maxSize;
			



			//std::queue<int> queSons;
			//queSons.push(rootId);

			//double maxNeutralitySize(1);
			//int curId = -1;
			//
			//while (!queSons.empty()||curId !=-1) {
			//	if (curId == -1) {
			//		curId = queSons.front();
			//		queSons.pop();
			//	}

			//	auto& cursons = sons[curId];
			//	std::sort(cursons.begin(), cursons.end(), [&](int a,int b) {
			//		
			//		});
			//}

			//


		}


		double getNBDvalue(const NBNinfo& nbnInfo,
			double bestRatio ) {
			

			auto& fitness = nbnInfo.m_vFitness;
			auto& belong = nbnInfo.m_belong;
			auto& dis2parent = nbnInfo.m_dis2parent;

			//optIds.clear();

			std::vector<double> nbdValues;


			auto norFit = fitness;
			ofec::dataNormalize(norFit);

			double fitRatio = (1.0 - bestRatio);
			int numBNodes(0);
			double totalNBD(0);
			for (int idx(0); idx < norFit.size(); ++idx) {
				if (norFit[idx] >= fitRatio && belong[idx] != -1&&belong[idx]!=idx) {
					nbdValues.push_back(dis2parent[idx]);
				}
			}


			double meanValue(0), stdValue(0);
			calMean(nbdValues, meanValue);
			calStd(nbdValues, meanValue, stdValue);
			return stdValue;

		}


		double getMaxNBD(const NBNinfo& nbnInfo) {
			std::vector<double> dis;
			for (int idx(0); idx < nbnInfo.m_solIds.size(); ++idx) {
				if (nbnInfo.m_belong[idx] != idx && nbnInfo.m_belong[idx] != -1) {
					dis.push_back(nbnInfo.m_dis2parent[idx]);
				}
			}

			double maxValue(0);
			calMax(dis, maxValue);
			return maxValue;
		}

		double getMaxNBD(const NBNinfo& nbnInfo, int& maxId) {
			double maxValue (std::numeric_limits<double>::lowest());
			maxId = -1;
			for (int idx(0); idx < nbnInfo.m_solIds.size(); ++idx) {
				if (nbnInfo.m_belong[idx] != idx && nbnInfo.m_belong[idx] != -1) {
					if (maxValue < nbnInfo.m_dis2parent[idx]) {
						maxId = idx;
						maxValue = nbnInfo.m_dis2parent[idx];
					}
				}
			}

			return maxValue;

		}

		double calculateMeanNBD(const NBNinfo& nbnInfo) {
			std::vector<double> dis;
			for (int idx(0); idx < nbnInfo.m_solIds.size(); ++idx) {
				if (nbnInfo.m_belong[idx] != idx && nbnInfo.m_belong[idx] != -1) {
					dis.push_back(nbnInfo.m_dis2parent[idx]);
				}
			}
			double meanValue(0);
			calMean(dis, meanValue);
			return meanValue;
		}

		double calculateMaxNBD(const NBNinfo& nbnInfo) {
			std::vector<double> dis;
			for (int idx(0); idx < nbnInfo.m_solIds.size(); ++idx) {
				if (nbnInfo.m_belong[idx] != idx && nbnInfo.m_belong[idx] != -1) {
					dis.push_back(nbnInfo.m_dis2parent[idx]);
				}
			}
			double meanValue(0);
			calMax(dis, meanValue);
			return meanValue;
		}

		double diffNBNRatio(const NBNinfo& cur, const NBNinfo& other) {
			int total(0);
			for (int idx(0); idx < cur.m_belong.size(); ++idx) {
				if (cur.m_belong[idx] == idx|| cur.m_belong[idx] == -1) {
					if (other.m_belong[idx] == idx || cur.m_belong[idx] == -1) {
						++total;
					}
				}
				else {
					if (cur.m_vFitness[other.m_belong[idx]] >= cur.m_vFitness[idx] 
						&& other.m_vFitness[cur.m_belong[idx]] >= other.m_vFitness[idx]) {
						++total;
					}
				}
			}
			return total / double(cur.m_belong.size());
		}


		void pathToRoot(int curId, const NBNinfo& nbnInfo, std::vector<int>& path) {
			
		//	std::cout << "new Path\t" << std::endl;
		//	std::cout <<"solSize\t" << nbnInfo.m_sols.size() << std::endl;
			path.push_back(curId);
		//	std::cout << "curId\t" << curId << std::endl;
		//	std::cout << "curId\t" << curId << "\tbelong\t" << nbnInfo.m_belong[curId] << std::endl;
			while (nbnInfo.m_belong[curId] != -1 && nbnInfo.m_belong[curId] != curId) {
				curId = nbnInfo.m_belong[curId];
				path.push_back(curId);
			//	std::cout << "curId\t" << curId <<"\tbelong\t"<<nbnInfo.m_belong[curId] << std::endl;
			}
			
		}

		double pathDistance(const NBNinfo& nbnInfo, const std::vector<int>& path) {
			double dis(0);
			for (auto& it : path) {
				if (nbnInfo.m_belong[it] != -1 && nbnInfo.m_belong[it] != it) {
					dis += nbnInfo.m_dis2parent[it];
				}
			}
			return dis;
		}



		void filterBestSolutions(std::vector<int>& solIds, const NBNinfo& nbnInfo,
			double fitnessThread ,
			double disThread ) {
			solIds.clear();
			auto fitness = UTILITY::valuesToSortedValues(nbnInfo.m_vFitness);
			dataNormalize(fitness);

			for (int idx(0); idx < fitness.size(); ++idx) {
				if (fitness[idx] >= fitnessThread && nbnInfo.m_dis2parent[idx] >= disThread) {
					solIds.push_back(idx);
				}
			}
		}


		void getBestSolIds(const NBNinfo& nbnInfo, std::vector<int>& solIds){
			double maxFit(0);
			calMax(nbnInfo.m_vFitness, maxFit);
			solIds.clear();
			for (int idx(0); idx < nbnInfo.m_vFitness.size(); ++idx) {
				if (nbnInfo.m_vFitness[idx] == maxFit) {
					solIds.push_back(idx);
				}
			}
		}
		void getSecondFurtherSolIds(const NBNinfo& nbnInfo, std::vector<int>& solIds) {
			solIds.clear();
			double maxDis = 1e9;
			double secondDis = 0;
			for (int idx(0); idx < nbnInfo.m_vFitness.size(); ++idx) {
				if (nbnInfo.m_dis2parent[idx]<maxDis) {
					if (secondDis < nbnInfo.m_dis2parent[idx]) {
						secondDis = nbnInfo.m_dis2parent[idx];
						solIds.clear();
						solIds.push_back(idx);
					}
					else if (secondDis == nbnInfo.m_dis2parent[idx]) {
						solIds.push_back(idx);
					}
				
				}
			}
		}



		
	}
}