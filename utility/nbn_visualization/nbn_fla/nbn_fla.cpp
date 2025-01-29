#include "nbn_fla.h"
#include "../../../core/problem/solution.h"
#include "../../../core/problem/continuous/continuous.h"
#include "../../../core/problem/problem.h"
#include "../../linear_algebra/vector.h"
#include "../../functional.h"
#include "../../function/custom_function.h"



const std::vector<double> ofec::NBN_FLA_info::m_radius_cut = { 0.1, 0.2, 0.3,0.5,0.7,0.9 };
void ofec::NBN_FLA_info::calculate(
	Problem *pro,
	Random *rnd,
	const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
	const std::vector<double>& fitness,
	const std::vector<int>& belong,
	const std::vector<double>& dis2parent)
{


	using namespace std;
	m_dim = pro->numberVariables();
	m_sample_size = sols.size();


	std::vector<int> curBelong = belong;
	auto curDis2Parent = dis2parent;
	{
		std::vector<int> rootId;
		for (int idx(0); idx < curBelong.size(); ++idx) {
			if (curBelong[idx] == idx) {
				rootId.push_back(idx);
			}
		}
		if (rootId.size() > 2) {
			std::sort(rootId.begin(), rootId.end(), [&](int a, int b) {
				return fitness[a] < fitness[b];
			});
			for (int idx(0); idx + 1 < rootId.size(); ++idx) {
				int id = rootId[idx];
				int parentId = rootId[rnd->uniform.nextNonStd<int>(idx + 1, rootId.size())];
				curBelong[id] = parentId;

				curDis2Parent[id] = pro->variableDistance(sols[id]->variableBase(), sols[parentId]->variableBase());
			}
		}
	}


	{
		std::vector<double> nbd;
		for (int idx(0); idx < sols.size(); ++idx) {
			//nbd[idx] = nbn_data[idx].m_dis2parent;
			if (curBelong[idx] != idx) {
				nbd.push_back(curDis2Parent[idx]);
			}

		}

		ofec::calMeanAndStd(nbd, m_mean_nbd, m_std_nbd);
		m_max_nbd = 0;
		for (auto& it : nbd) {
			m_max_nbd = std::max(it, m_max_nbd);
		}

		double threadhold = m_mean_nbd + 3 * m_std_nbd;
		m_numOuterOptima = 0;
		for (auto& it : nbd) {
			if (it > threadhold) {
				++m_numOuterOptima;
			}
		}

	}




	std::vector<std::vector<int>> sons(sols.size());
	for (int idx(0); idx < sols.size(); ++idx) {
		if (belong[idx] != idx) {
			sons[belong[idx]].push_back(idx);
		}
	}

	// calculate rugeness

	{

		double ratio = 2.0 / 3.0;
		m_nbn_rug_ic.clear();
		{

			double totolRugRatio(0), maxRugRatio(0);


			int totalRugEdge(0), totalEdge(0);
			int curRugEdge(0), curNumEdge(0);
			std::vector<int> curNeighbors;

			for (int idx(0); idx < sols.size(); ++idx) {
				if (belong[idx] != idx) {
					curNeighbors.clear();
					int parentId = belong[idx];
					int curId = idx;
					double disThreadhold = pro->variableDistance(sols[curId]->variableBase(), sols[parentId]->variableBase()) * ratio;
					//double disThreadhold = sols[curId]->variableDistance(*sols[parentId], pro) * ratio;
					curNeighbors.clear();
					for (auto& it : sons[parentId]) {
						curNeighbors.push_back(it);
					}
					for (auto& it : sons[curId]) {
						curNeighbors.push_back(it);
					}
					curNumEdge = curNeighbors.size();
					curRugEdge = 0;
					for (auto& it : curNeighbors) {
						if (fitness[it] < fitness[curId]) {
							double dis1 = pro->variableDistance(sols[parentId]->variableBase(), sols[it]->variableBase());

							double dis2 = pro->variableDistance(sols[curId]->variableBase(), sols[it]->variableBase());

						//	double dis1 = sols[it]->variableDistance(*sols[parentId], pro);
						//	double dis2 = sols[it]->variableDistance(*sols[curId], pro);
							if (dis1 < disThreadhold && dis2 < disThreadhold) {
								++curRugEdge;
							}
						}
					}
					totalRugEdge += curRugEdge;
					totalEdge += curNumEdge;
					maxRugRatio = std::max<double>(maxRugRatio, double(curRugEdge) / double(curNumEdge));

				}
			}


			totolRugRatio = double(totalRugEdge) / double(totalEdge);

			m_nbn_rug_ic.push_back(maxRugRatio);
			m_nbn_rug_ic.push_back(totolRugRatio);
		}


		{

			double totolRugRatio(0), maxRugRatio(0);



			int totalRugEdge(0), totalEdge(0);
			int curRugEdge(0), curNumEdge(0);
			std::vector<int> curNeighbors;

			for (int idx(0); idx < sols.size(); ++idx) {
				if (belong[idx] != idx) {
					curNeighbors.clear();
					int parentId = belong[idx];
					int curId = idx;
					//double disThreadhold = sols[curId]->variableDistance(*sols[parentId], pro) * ratio;
					double disThreadhold = pro->variableDistance(sols[parentId]->variableBase(), sols[curId]->variableBase()) * ratio;
					curNeighbors.clear();
					for (auto& it : sons[parentId]) {
						curNeighbors.push_back(it);
					}
					curNumEdge = curNeighbors.size();
					curRugEdge = 0;
					for (auto& it : curNeighbors) {
						if (fitness[it] < fitness[curId]) {
							double dis1 = pro->variableDistance(sols[parentId]->variableBase(), sols[it]->variableBase());
							double dis2 = pro->variableDistance(sols[curId]->variableBase(), sols[it]->variableBase());


							//double dis1 = sols[it]->variableDistance(*sols[parentId], pro);
							//double dis2 = sols[it]->variableDistance(*sols[curId], pro);
							if (dis1 < disThreadhold && dis2 < disThreadhold) {
								++curRugEdge;
							}
						}
					}
					totalRugEdge += curRugEdge;
					totalEdge += curNumEdge;
					maxRugRatio = std::max<double>(maxRugRatio, double(curRugEdge) / double(curNumEdge));

				}
			}


			totolRugRatio = double(totalRugEdge) / double(totalEdge);

			m_nbn_rug_ic.push_back(maxRugRatio);
			m_nbn_rug_ic.push_back(totolRugRatio);
		}

	}
	{	
		m_totalBasin.resize(sons.size());
		std::fill(m_totalBasin.begin(), m_totalBasin.end(), 1);
		
		std::queue<int> queInt;
		for (int idx(0); idx < sons.size(); ++idx) {
			if (sons[idx].empty()) {
				queInt.push(idx);
			}
		}

		while (!queInt.empty()) {
			int cur = queInt.front();
			queInt.pop();
			m_totalBasin[belong[cur]] += m_totalBasin[cur];
			 
		}
	}

	for (int idx(0); idx < sols.size(); ++idx) {
		if (dis2parent[idx] > m_basin_radius_cut) {
			optIds.push_back(idx);
		}
	}

	{
		std::vector<int> visited(sols.size(), false);
		basin_idxs.resize(optIds.size());
		std::vector<int> belong(sols.size(),-1);;
		for (auto& it : optIds) {
			visited[it] = true;

		}
		std::queue<int> queInt;

		for (int idx(0); idx < optIds.size(); ++idx) {
		//	basin_idxs[idx].push_back(optIds[idx]);
			queInt.push(optIds[idx]);
			belong[optIds[idx]] = idx;
		}
		
		while (!queInt.empty()) {
			int cur = queInt.front();
			queInt.pop();
			for (auto& sonId : sons[cur]) {
				if (!visited[sonId]) {
					queInt.push(sonId);
					belong[sonId] = belong[cur];
				}
			}
		}
		for (int idx(0); idx < belong.size(); ++idx) {
			basin_idxs[belong[idx]].push_back(idx);
		}

	}



	std::vector<double> norFit = fitness;
	double maxFit(norFit.front()), minFit(norFit.front());
	{
		for (auto& it : norFit) {
			maxFit = std::max(it, maxFit);
			minFit = std::min(it, minFit);
		}
		for (auto& it : norFit) {
			it = ofec::mapReal<double>(it, minFit, maxFit, 0, 1);
		}
	}

	//int dim_div = std::max(2.0, exp(log(double(numSample_int)) / double(GET_PRO()));
	{

		{
			std::vector<double> effective_nbd;
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] != idx) {
					effective_nbd.push_back(dis2parent[idx]);
				}
			}
			double avg_rug(0);
			ofec::calStd(effective_nbd, avg_rug, m_nbd_rug);



		}

		{
			double radius_threadhold(0);
			int numSample_int = sols.size();
			auto proCon = dynamic_cast<ofec::Continuous*>(pro);
			int dim_div = std::max<int>(2, exp(log(double(numSample_int)) / double(proCon->numberVariables())));
			if (dim_div > 2) {
				radius_threadhold = 1.0 / double(dim_div) * 3.0 / 1.1;

				//radius_threadhold = 1.0 / double(dim_div) * 5.0 / 1.1;
			}
			else radius_threadhold = 1.0 / double(dim_div) * 2.0 / 1.1;


			std::vector<double> effective_nbd;
			for (int idx(0); idx < belong.size(); ++idx) {
				if (belong[idx] != idx&&dis2parent[idx]< radius_threadhold) {
					effective_nbd.push_back(dis2parent[idx]);
				}
			}
			double avg_rug(0);
			ofec::calStd(effective_nbd, avg_rug, m_nbd_rug_threadHold);

		}


		std::vector<double> son_fit;
		std::vector<double> son_dif_fit;
		std::vector<double> son_dis;
		m_maxSonStdDis = 0;
		m_maxSonStdFit = 0;
		m_maxNumSons = 0;



		m_rudgeness = 0;


		m_maxSonSonIdxs = 0;
		m_sumSonDis = 0;
		m_sumSonSonDis = 0;

		int curSons(0);
		double curStd;
		double curSumSonDis = 0;
		double curSonSonSumDis(0);
		for (int idx(0); idx < sons.size(); ++idx) {
			auto& it(sons[idx]);
			m_maxNumSons = std::max<int>(m_maxNumSons, it.size());
			curSons = 0;
			curSons += it.size();
			son_fit.clear();
			son_dis.clear();
			son_dif_fit.clear();
			curSumSonDis = 0;
			curSonSonSumDis = 0;
			for (auto& sonId : it) {
				curSumSonDis += dis2parent[sonId];
				curSonSonSumDis += dis2parent[sonId];
				curSons += sons[sonId].size();
				son_fit.push_back(norFit[sonId]);
				son_dis.push_back(dis2parent[sonId]);
				son_dif_fit.push_back((norFit[idx]-norFit[sonId]));
				for (auto& sonsonId : sons[sonId]) {
					curSonSonSumDis += dis2parent[sonsonId];
					//double curFit = ofec::mapReal<double>(nbn_data[sonsonId].m_fitness, minFit, maxFit, 0, 1);
					son_fit.push_back(norFit[sonsonId]);
					son_dis.push_back(dis2parent[sonsonId]);
					//son_dif_fit.push_back((norFit[sonId] - norFit[sonsonId])/ dis2parent[sonsonId]);
					//son_dif_fit.push_back((norFit[sonId] - norFit[sonsonId]) );
				}
			}
			double meanVal(0);
			ofec::calMeanAndStd(son_fit, meanVal, curStd);
			m_maxSonStdFit = std::max(m_maxSonStdFit, curStd);


			ofec::calMeanAndStd(son_dis, meanVal, curStd);
			m_maxSonStdDis = std::max(m_maxSonStdDis, curStd);

			m_maxSonSonIdxs = std::max(m_maxSonSonIdxs, curSons);
			ofec::calMeanAndStd(son_dif_fit, meanVal, curStd);
			m_rudgeness = std::max(m_rudgeness, curStd);
			m_sumSonDis = std::max(curSumSonDis, m_sumSonDis);
			m_sumSonSonDis = std::max(m_sumSonSonDis, curSonSonSumDis);



		}

	}

	std::vector<double> nbd = curDis2Parent;
	for (int idx(0); idx < nbd.size(); ++idx) {
		if (belong[idx] == idx) {
			nbd[idx] = 0;
		}
	}
	{
		//int numSample_int = nbn_data.size();
		double dim_div = std::max(1.0, exp(log(double(sols.size())) / double(m_dim)));
		double dis = 1.0 / double(dim_div) * sqrt(dim_div);
		//std::vector<int> numSubOpt(nbn_data.size(),0);

		auto& numSubOpt(m_numSubOpt);
		numSubOpt.resize(m_radius_cut.size(), 0);
		std::fill(numSubOpt.begin(), numSubOpt.end(), 0);

		auto& maxBasinSize(m_maxBasinSize);
		maxBasinSize.resize(m_radius_cut.size(), 0);
		std::fill(maxBasinSize.begin(), maxBasinSize.end(), 0);


		auto& minBasinSize(m_minBasinSize);
		minBasinSize.resize(m_radius_cut.size(), 0);
		std::fill(minBasinSize.begin(), minBasinSize.end(), std::numeric_limits<double>::max());


		std::vector<int> basin_size(sols.size(), 1);
		std::vector<int> neurtual_size(sols.size(), 1);
		std::vector<std::vector<double>>  local_opt_basin_size(m_radius_cut.size());
		std::vector<std::vector<int>> local_opt_leanNum(m_radius_cut.size(), std::vector<int>(sols.size(), 1));
		std::queue<int> que_nodes;
		std::vector<int> visited(sols.size(), 0);
		for (int idx(0); idx < sons.size(); ++idx) {
			visited[idx] = sons[idx].size();
			if (visited[idx] == 0) {
				que_nodes.push(idx);
			}
		}
		std::vector<double> local_optBasicSize(m_radius_cut.size());


		while (!que_nodes.empty()) {
			int cur = que_nodes.front();
			que_nodes.pop();

			for (auto& sonId : sons[cur]) {
				basin_size[cur] += (basin_size[sonId]);
				if (/*dis2parent[sonId] <= dis && */abs(norFit[cur] - norFit[sonId]) <= m_ruggness_eps* dis2parent[sonId]) {
					neurtual_size[cur] += neurtual_size[sonId];
				}
				for (int idx(0); idx < m_radius_cut.size(); ++idx) {
					const auto& radius = m_radius_cut[idx];
					if (dis2parent[sonId] <= radius) {
						local_opt_leanNum[idx][cur] += local_opt_leanNum[idx][sonId];
					}
				}
			}

			for (int idx(0); idx < m_radius_cut.size(); ++idx) {
				const auto& radius = m_radius_cut[idx];
				if (dis2parent[cur] > radius) {
					numSubOpt[idx]++;
					double basinRatio = double(local_opt_leanNum[idx][cur]) / sols.size();
					//		maxBasinSize[idx] = std::max(maxBasinSize[idx], basinRatio);
					//		minBasinSize[idx] = std::min(minBasinSize[idx], basinRatio);
					local_opt_basin_size[idx].push_back(basinRatio);
				}
			}

			if (--visited[belong[cur]] == 0) {
				que_nodes.push(belong[cur]);
			}


		}

		m_maxNeutural = 0;
		m_maxBasinDis = 0;

		for (int idx(0); idx < sols.size(); ++idx) {
			m_maxNeutural = std::max<double>(m_maxNeutural, double(neurtual_size[idx]) / sols.size());;
			m_maxBasinDis = std::max<double>(m_maxBasinDis, double(basin_size[idx]) / sols.size() * nbd[idx]);;
		}

		auto& stdBasinSize = m_stdBasinSize;
		stdBasinSize.resize(m_radius_cut.size());
		for (int idx(0); idx < m_radius_cut.size(); ++idx) {
			for (int idB(0); idB < local_opt_basin_size[idx].size(); ++idB) {
				double& basinRatio = local_opt_basin_size[idx][idB];
				maxBasinSize[idx] = std::max(maxBasinSize[idx], basinRatio);
				minBasinSize[idx] = std::min(minBasinSize[idx], basinRatio);
			}
			double meanVal(0);
			ofec::calMeanAndStd(local_opt_basin_size[idx], meanVal, stdBasinSize[idx]);

			if (local_opt_basin_size[idx].empty()) {
				maxBasinSize[idx] = minBasinSize[idx] = stdBasinSize[idx] = 0;
			}
			//.push_back(basinRatio);
		}
	}


	// symmetrix
	std::vector<double> angle;
	if (dynamic_cast<Continuous*>(pro)!=nullptr) {
		int p1(0), p2(0), p3(0);
		// p1->p2 , p2->p3, angle of the two 
		for (int idx(0); idx < belong.size(); ++idx) {
			p1 = idx;
			p2 = belong[p1];
			p3 = belong[p2];
			if (p2 != p3) {
				const VariableVector<Real>& x1 = dynamic_cast<const ofec::Solution<>&>(*sols[p1]).variable();
				const VariableVector<Real>& x2 = dynamic_cast<const ofec::Solution<>&>(*sols[p2]).variable();
				const VariableVector<Real>& x3 = dynamic_cast<const ofec::Solution<>&>(*sols[p3]).variable();

				ofec::Vector v1(x1.vector()), v2(x2.vector()), v3(x3.vector());
				ofec::Vector l1 = v2 - v1;
				ofec::Vector l2 = v3 - v2;

				double cosangle = (l1 * l2) / (l1.norm()*l2.norm());
			
				angle.push_back(cosangle);
			}
		}

		for (auto& curangle : angle) {
			m_maxangle = std::max(curangle, m_maxangle);

		}
		ofec::calMeanAndStd(angle, m_meanangle, m_stdangle);

	}
}



//void ofec::NBN_FLA_info::calBasinSize(std::vector<int>& belong,
//	std::vector<Real>& basin_size) {
//
//	std::vector<std::vector<int>> sons(belong.size());
//	for (int idx(0); idx < belong.size(); ++idx) {
//		if (belong[idx] != idx) {
//			sons[belong[idx]].push_back(idx);
//		}
//	}
//
//
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
//
//	while (!que_nodes.empty()) {
//		int cur = que_nodes.front();
//		que_nodes.pop();
//
//		for (auto& sonId : sons[cur]) {
//			basin_size[cur] += (basin_size[sonId]);
//		}
//
//		if (--visited[belong[cur]] == 0) {
//			que_nodes.push(belong[cur]);
//		}
//	}
//}
//
//
//
//void ofec::NBN_FLA_info::getBasinTreeDistanceRatio(
//	const std::vector<int>& belong,
//	const std::vector<Real>& dis2parent,
//	std::vector<Real>& distanceRatio
//) {
//
//	distanceRatio.resize(belong.size());
//	std::vector<std::vector<int>> basin_idxs;
//	getDirectBasin(belong, dis2parent, basin_idxs);
//
//	std::vector<Real> distance(dis2parent);
//	double maxDis;
//	for (auto& it : distance) {
//		if (it < 1e9) {
//			maxDis = std::max(maxDis, it);
//		}
//	}
//	maxDis *= 1.1;
//	for (auto& it : distance) {
//		if (it > 1e9) it = maxDis;
//	}
//
//	for (int idx(0); idx < distance.size(); ++idx) {
//		if (distance[idx] == 0) {
//			distance[idx] = distance[belong[idx]];
//			//std::cout << "distacne 0 \t" << idx << "\tbelong\t" << belong[idx] << std::endl;
//		}
//	}
//
//	std::vector<Real> sonDistances;
//	std::vector<Real> disRatio;
//
//
//	std::vector<Real> avgSonDistances;
//
//	for (int idx(0); idx < belong.size(); ++idx) {
//		if (basin_idxs[idx].empty())continue;
//		double avgSonDis = 0;
//		sonDistances.clear();
//		for (auto& it : basin_idxs[idx]) {
//			sonDistances.push_back(distance[it]);
//		}
//		calMean(sonDistances, avgSonDis);
//		
//		
//		distanceRatio[idx] = distance[idx] / avgSonDis;
//		avgSonDistances.push_back(avgSonDis);
//		
//		disRatio.push_back(distanceRatio[idx]);
//	}
//
//	double minDisRatio(0);
//	calMin<double,double>(disRatio, minDisRatio);
//	minDisRatio *= 0.9;
//	for (int idx(0); idx < belong.size(); ++idx) {
//		if (basin_idxs[idx].empty()) {
//			distanceRatio[idx] = minDisRatio;
//		}
//	}
//
//	//{
//	//	double maxDistanceRatio(0);
//	//	double minDistanceRatio(0);
//	//	calMax(distanceRatio, maxDistanceRatio);
//	//	calMin(distanceRatio, minDistanceRatio);
//
//	////	std::cout << "max Distnace ratio\t" << maxDistanceRatio << std::endl;
//	////	std::cout << "min distance ratio\t" << minDistanceRatio << std::endl;
//	//}
//
//
//	//{
//	//	double minValue(0), maxValue(0);
//	//	calMax(avgSonDistances, maxValue);
//	//	calMin(avgSonDistances, minValue);
//	////	std::cout << "max avgSonDistances \t" << maxValue << std::endl;
//	////	std::cout << "min avgSonDistances \t" << minValue << std::endl;
//	//}
//
//
//	//{
//	//	double minValue(0), maxValue(0);
//	//	calMax(distance, maxValue);
//	//	calMin(distance, minValue);
//	////	std::cout << "max distance \t" << maxValue << std::endl;
//	////	std::cout << "min distance \t" << minValue << std::endl;
//	//}
//
//}
//
//
//void ofec::NBN_FLA_info::calculateBasins(
//	const std::vector<std::vector<int>>& selectedCenters,
//	const std::vector<int>& belong,
//	const std::vector<Real>& dis2parent,
//	std::vector<std::vector<int>>& basins
//) {
//	std::vector<std::vector<int>> direct_sons;
//	getDirectBasin(belong, dis2parent, direct_sons);
//	std::vector<int> belongBasinId(belong.size(),-1);
//	basins.resize(selectedCenters.size());
//	
//
//	std::queue<int> queueIds;
//	for (int idx(0); idx < selectedCenters.size(); ++idx) {
//		for (auto& it : selectedCenters[idx]) {
//			if (it < belong.size()) {
//				if (belongBasinId[it] == -1) {
//					belongBasinId[it] = idx;
//					queueIds.push(it);
//					basins[idx].push_back(it);
//				}
//			}
//		}
//	}
//
//	while (!queueIds.empty()) {
//		int cur = queueIds.front();
//		int idx = belongBasinId[cur];
//		queueIds.pop();
//		for (auto& it : direct_sons[cur]) {
//			if (belongBasinId[it] == -1) {
//				belongBasinId[it] = idx;
//				queueIds.push(it);
//				basins[idx].push_back(it);
//			}
//		}
//	}
//
//
//}
//
//void ofec::NBN_FLA_info::calculateBasinBestIds(const std::vector<std::vector<int>>& basins, 
//	const std::vector<Real>& fitness, std::vector<int>& basinBestIds)
//{
//	basinBestIds.clear();
//	//basinBestIds.resize(basins.size());
//	
//	for (int idx(0); idx < basinBestIds.size(); ++idx) {
//		Real bestFit = std::numeric_limits<Real>::lowest();
//		std::vector<int> bestIds;
//		for (auto& it : basins[idx]) {
//			if (bestFit < fitness[idx]) {
//				bestFit = fitness[idx];
//				bestIds.clear();
//				bestIds.push_back(idx);
//			}
//			else if (bestFit == fitness[idx]) {
//				bestIds.push_back(idx);
//			}
//			
//		}
//
//		for (auto& it : bestIds) {
//			basinBestIds.push_back(it);
//		}
//	}
//}
//
//
//
//void ofec::NBN_FLA_info::getBasinSize(
//	const std::vector<int>& belong,
//	std::vector<std::vector<int>>& sons,
//	std::vector<int>& basinSizes) {
//
//
//
//	std::vector<int> visited(sons.size(), 0);
//	for (int idx(0); idx < visited.size(); ++idx) {
//		visited[idx] = sons[idx].size();
//	}
//	basinSizes.resize(sons.size());
//	std::fill(basinSizes.begin(), basinSizes.end(), 1);
//	std::queue<int> queLeaves;
//	for (int idx(0); idx < basinSizes.size(); ++idx) {
//		if (sons[idx].empty()) {
//			queLeaves.push(idx);
//		}
//	}
//
//	while (!queLeaves.empty()) {
//		int cur = queLeaves.front();
//		queLeaves.pop();
//		if (belong[cur] != -1) {
//			basinSizes[belong[cur]] += basinSizes[cur];
//			if (--visited[belong[cur]] == 0) {
//				queLeaves.push(belong[cur]);
//			}
//		}
//	}
//
//}
//
//void ofec::NBN_FLA_info::divideBasinByDistanceBasinSize(
//	const std::vector<int>& belong,
//	const std::vector<Real>& dis2parent,
//	double distanceThreadhold,
//	int basinSizeThreadhold,
//	std::vector<std::vector<int>>& basins) {
//
//	std::vector<int> basinSize;
//	std::vector<std::vector<int>> sons;
//
//	getDirectBasin(belong, dis2parent, sons);
//	getBasinSize(belong, sons, basinSize);
//
//	basins.clear();
//	std::vector<int> sortedIds;
//
//
//	int rootId = -1;
//
//
//	for (int idx(0); idx < belong.size();++idx)
//	{
//		if (belong[idx] != -1)
//			sortedIds.push_back(idx);
//		else rootId = idx;
//	}
//	std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
//		return dis2parent[a] > dis2parent[b];
//		});
//	
//	std::vector<bool> flagFeasible(belong.size());
//	std::fill(flagFeasible.begin(), flagFeasible.end(), true);
//
//
//	std::vector<std::set<int>>  set_sons(sons.size());
//	for (int idx(0); idx < sons.size(); ++idx) {
//		for (auto& it : sons[idx]) {
//			set_sons[idx].insert(it);
//		}
//	}
//
//
//	for (auto& it : sortedIds) {
//		if (flagFeasible[it]
//			&& dis2parent[it] >= distanceThreadhold 
//			&& basinSize[it] >= basinSizeThreadhold) {
//
//
//			basins.push_back(std::vector<int>());
//			getSubBasin(set_sons, it, basins.back());
//			for (auto& it2 : basins.back()) {
//				flagFeasible[it2] = false;
//			}
//			if (belong[it] != -1) {
//				set_sons[belong[it]].erase(it);
//			}
//
//			int parentId = belong[it];
//			while (parentId != -1) {
//				basinSize[parentId] -= basinSize[it];
//				parentId = belong[parentId];
//			}
//		}
//	}
//
//	basins.push_back(std::vector<int>());
//	getSubBasin(set_sons, rootId, basins.back());
//	
//
//	
//
//
//}
