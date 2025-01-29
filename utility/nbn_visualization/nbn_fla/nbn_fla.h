#ifndef NBN_FLA_H
#define NBN_FLA_H

#include <vector>
#include <queue>
#include <ostream>
#include <set>


#include "../../../core/random/newran.h"
#include "../../../core/problem/solution.h"

namespace ofec {
	class Problem;

	class NBN_FLA_info {
	public:

		double m_ruggness_eps = 0.0001;

		int m_dim = 0;
		int m_sample_size = 0;

		int m_funId = -1;
		int m_instanceId = -1;

		// global info
		double m_std_nbd = 0;
		double m_max_nbd = 0;
		double m_mean_nbd = 0;

		double m_maxNumSons = 0;
		double m_maxBasinDis = 0;

		int m_numOuterOptima = 0;


		double m_maxSonStdDis = 0;
		double m_maxSonStdFit = 0;
		double m_maxNeutural = 0;



		double m_maxangle = 0;
		double m_meanangle = 0;
		double m_stdangle = 0;


		const static std::vector<double> m_radius_cut;

		std::vector<int> m_numSubOpt;
		std::vector<double> m_maxBasinSize;
		std::vector<double> m_minBasinSize;
		std::vector<double> m_stdBasinSize;

		double m_rudgeness = 0;
		double m_nbd_rug = 0;
		double m_nbd_rug_threadHold = 0;

		std::vector<double> m_nbn_rug_ic;


		double m_basin_radius_cut = 0.01;
		std::vector<std::vector<int>> basin_idxs;

		std::vector<int> m_totalBasin;

		std::vector<int> optIds;



		// for rugeness
		int m_maxSonSonIdxs = 0;
		double m_sumSonDis = 0;
		double m_sumSonSonDis = 0;

		static 	void outputHead(std::ostream& out) {
			//out << "filename" << "," << "dim" << ",";
			//out << "fdc" << "," << "std_nbd" << "," << "max_nbd" << "," << "maxNumSons" << "," << "maxBasinDis" << ",";
			//out << "random_walk_Neutural" << "," << "direciton_random_walk" << ",";
			//out << "maxNeutural" << ",";
			//out << "random_walk_rug" << "," << "direciton_random_walk_rug" << ",";
			//out << "maxSonStdDis" << "," << "maxSonStdFit" << ",";

			//for (int idx(0); idx < m_radius_cut.size(); ++idx) {
			//	out << m_radius_cut[idx] << "_numSubOpt,maxBasinSize,minBasinSize,stdBasinSize,";
			//}
			//out << std::endl;

			out << "funId" << "," << "instanceId" << ",";
			out << "dim" << ", " << "sample_size" << ", ";
			out << "std_nbd" << "," << "max_nbd" << "," << "mean_nbd" << ",";
			out << "maxNumSons" << "," << "maxBasinDis" << "," << "numOuterOptima" << ",";
			out << "maxSonStdDis" << "," << "maxSonStdFit" << "," << "maxNeutural" << ",";
			out << "max_angle" << "," << "mean_angle" << "," << "std_angle" << ",";


			for (int idx(0); idx < m_radius_cut.size(); ++idx) {
				out << "numSubOpt" << ",";
				out << "maxBasinSize" << ",";
				out << "minBasinSize" << ",";
				out << "stdBasinSize" << ",";
			}
			out << std::endl;
			out.flush();

		}
		void output(std::ostream& out) {
			out << m_funId << "," << m_instanceId << ",";
			out << m_dim << "," << m_sample_size << ",";
			out << m_std_nbd << "," << m_max_nbd << "," << m_mean_nbd << ",";
			out << m_maxNumSons << "," << m_maxBasinDis << "," << m_numOuterOptima << ",";
			out << m_maxSonStdDis << "," << m_maxSonStdFit << "," << m_maxNeutural << ",";
			out << m_maxangle << "," << m_meanangle << "," << m_stdangle << ",";


			for (int idx(0); idx < m_radius_cut.size(); ++idx) {
				out << m_numSubOpt[idx] << ",";
				out << m_maxBasinSize[idx] << ",";
				out << m_minBasinSize[idx] << ",";
				out << m_stdBasinSize[idx] << ",";
			}
			out << std::endl;

			out.flush();

		}


		void calculate(
			ofec::Problem* pro,
			Random* rnd,
			const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
			const std::vector<double>& fitness,
			const std::vector<int>& belong,
			const std::vector<double>& dis2parent);


		//static double analyzeDiffFL(
		//	const std::vector<int>& belong,
		//	const std::vector<double>& fitness) {
		//	int numSame(0);

		//	for (int idx(0); idx < fitness.size(); ++idx) {
		//		if (fitness[idx] <= fitness[belong[idx]]) {
		//			++numSame;
		//		}
		//	}
		//	return double(numSame) / fitness.size();
		//}


		//static bool judgeNBN(
		//	const std::vector<int>& belong,
		//	const std::vector<double>& fitness) {
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//		if (fitness[idx] > fitness[belong[idx]]) {
		//			return false;
		//		}
		//	}
		//	return true;
		//}


		//static void calBasinSize(std::vector<int>& belong,
		//	std::vector<double>& basin);

		//static void calBasins(const std::vector<int>& belong,
		//	const std::vector<double>& dis2parent,
		//	double m_basin_radius_cut,
		//	std::vector<int>& optIds,
		//	std::vector<int>& belongBid,
		//	std::vector<std::vector<int>>& basin_idxs) {
		//	std::vector<std::vector<int>> sons(belong.size());
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//		if (belong[idx] != idx) {
		//			sons[belong[idx]].push_back(idx);
		//		}
		//	}
		//	optIds.clear();
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//		if (dis2parent[idx] > m_basin_radius_cut) {
		//			optIds.push_back(idx);
		//		}
		//	}

		//	{
		//		std::vector<int> visited(belong.size(), false);
		//		basin_idxs.resize(optIds.size());
		//		//std::vector<int> belongBid(belong.size(), -1);;

		//		belongBid.resize(belong.size(), -1);;
		//		std::fill(belongBid.begin(), belongBid.end(), -1);

		//		for (auto& it : optIds) {
		//			visited[it] = true;

		//		}
		//		std::queue<int> queInt;

		//		for (int idx(0); idx < optIds.size(); ++idx) {
		//			//	basin_idxs[idx].push_back(optIds[idx]);
		//			queInt.push(optIds[idx]);
		//			belongBid[optIds[idx]] = idx;
		//		}

		//		while (!queInt.empty()) {
		//			int cur = queInt.front();
		//			queInt.pop();
		//			for (auto& sonId : sons[cur]) {
		//				if (!visited[sonId]) {
		//					queInt.push(sonId);
		//					belongBid[sonId] = belongBid[cur];
		//				}
		//			}
		//		}
		//		for (int idx(0); idx < belong.size(); ++idx) {
		//			basin_idxs[belongBid[idx]].push_back(idx);
		//		}


		//		// for test 

		//		std::cout << "begin test" << std::endl;

		//		int stop = -1;


		//		for (int idx(0); idx < belong.size(); ++idx) {
		//			if (belongBid[idx] != belongBid[belong[idx]] && !visited[idx]) {
		//				int stop = -1;
		//				std::cout << idx << std::endl;
		//			}
		//		}

		//		std::cout << std::endl << std::endl;

		//	}
		//}


		//static void getBasin(const std::vector<int>& belong,
		//	int peakId, std::vector<int>& curBasin) {
		//	//std::vector<std::vector<int>> sons;
		//	curBasin.clear();
		//	std::vector<std::vector<int>> sons(belong.size());
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//		if (belong[idx] != idx) {
		//			sons[belong[idx]].push_back(idx);
		//		}
		//	}

		//	std::queue<int> que;
		//	que.push(peakId);
		//	while (!que.empty()) {
		//		int cur = que.front();
		//		que.pop();
		//		curBasin.push_back(cur);
		//		for (auto& it : sons[cur]) {
		//			que.push(it);
		//		}
		//	}
		//}



		//static void getSubBasin(const std::vector<std::vector<int>>& sons,
		//	int peakId, std::vector<int>& curBasin) {
		//	curBasin.clear();
		//	std::queue<int> que;
		//	que.push(peakId);
		//	while (!que.empty()) {
		//		int cur = que.front();
		//		que.pop();
		//		curBasin.push_back(cur);
		//		for (auto& it : sons[cur]) {
		//			que.push(it);
		//		}
		//	}
		//}
		//

		//static void getSubBasin(const std::vector<std::set<int>>& sons,
		//	int peakId, std::vector<int>& curBasin) {
		//	curBasin.clear();
		//	std::queue<int> que;
		//	que.push(peakId);
		//	while (!que.empty()) {
		//		int cur = que.front();
		//		que.pop();
		//		curBasin.push_back(cur);
		//		for (auto& it : sons[cur]) {
		//			que.push(it);
		//		}
		//	}
		//}
		//

		//static void getDirectBasin(
		//	const std::vector<int>& belong,
		//	const std::vector<Real>& dis2parent,
		//	std::vector<std::vector<int>>& basin_idxs
		//	) {
		//	basin_idxs.resize(belong.size());
		//	for (int idx(0); idx < belong.size(); ++idx) {
		//	    if(belong[idx]!=-1)
		//		basin_idxs[belong[idx]].push_back(idx);
		//	}
		//}

		//static void getBasinSize(
		//	const std::vector<int>& belong,
		//	std::vector<std::vector<int>>& sons,
		//	std::vector<int>& basinSizes);
	

		//static void getBasinTreeDistanceRatio(
		//	const std::vector<int>& belong,
		//	const std::vector<Real>& dis2parent,
		//	std::vector<Real>& distanceRatio
		//);


		//static void calculateBasins(
		//	const std::vector<std::vector<int>>& selectedCenters,
		//	const std::vector<int>& belong,
		//	const std::vector<Real>& dis2parent,
		//	std::vector<std::vector<int>>& basins
		//);

		//static void calculateBasinBestIds(
		//	const std::vector<std::vector<int>>& basins,
		//	const std::vector<Real>& fitness,
		//	std::vector<int>& basinBestIds
		//);


		//static void divideBasinByDistanceBasinSize(
		//	const std::vector<int>& belong,
		//	const std::vector<Real>& dis2parent,
		//	double distanceThreadhold,
		//	int basinSizeThreadhold,
		//	std::vector<std::vector<int>>& basins);

		
	};
//	const std::vector<double> NBN_FLA_info::m_radius_cut = { 0.1, 0.2, 0.3,0.5,0.7,0.9 };
}

#endif // ! NBN_FLA_H




