#include "nbn_iteration_multithread.h"
#include "nbn_algorithm_utility.h"
#include <mutex>
#include <thread>
#include <chrono>

namespace ofec {
	namespace nbn {



		void updateNearsetBetterSolution(
			int curSolId,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd) {
			for (int idx(0); idx < sols.size(); ++idx) {
				if (idx != curSolId) {
					if (fitness[idx] > fitness[curSolId]) {
						double dis = sols[idx]->variableDistance(*sols[curSolId], env);
						if (dis2parent[curSolId] > dis) {
							dis2parent[curSolId] = dis;
							belong[curSolId] = idx;
						}
						else if (dis2parent[curSolId] == dis && rnd->uniform.next() < 0.5) {
							belong[curSolId] = idx;
						}
					}
	/*				else if (fitness[idx] < fitness[curSolId]) {
						double dis = sols[idx]->variableDistance(*sols[curSolId], env);
						if (dis2parent[idx] > dis) {
							dis2parent[idx] = dis;
							belong[idx] = curSolId;
						}
						else if (dis2parent[curSolId] == dis && rnd->uniform.next() < 0.5) {
							belong[idx] = curSolId;
						}
					}*/
				}
			}
		}


		void updateNearsetBetterSolutionThreadTask(
			std::vector<int>& solIds,
			std::mutex& idMtx,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd) {
			int curId(-1);
			while (true) {
				curId = -1;
				{
					idMtx.lock();
					if (!solIds.empty()) {
						curId = solIds.back();
						solIds.pop_back();
					}
					idMtx.unlock();
				}
				if (curId ==-1)break;
				updateNearsetBetterSolution(
					curId, sols,belong,dis2parent,fitness, env,rnd);
			
			}
			
		}


		void updateNearsetBetterSolutions(
			const std::vector<int>& solIds,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd) {


			belong.resize(sols.size());
			dis2parent.resize(sols.size());
			std::fill(belong.begin(), belong.end(), -1);
			std::fill(dis2parent.begin(), dis2parent.end(), std::numeric_limits<double>::max());

			int maxId = 0;
			std::mutex idMtx;
			int numThread = std::thread::hardware_concurrency();
			
			std::vector<std::vector<int>> belongs(numThread, belong);
			std::vector<std::vector<double>> dis2parents(numThread, dis2parent);

			std::vector<std::shared_ptr<Random>> rnds(numThread);
			for (auto& it : rnds) {
				it.reset(new Random(rnd->uniform.next()));
			}


			auto refSolIds = solIds;
			{
				std::vector<std::thread> thrds;
				for (size_t i = 0; i < numThread; ++i) {
					thrds.push_back(std::thread(
						&updateNearsetBetterSolutionThreadTask,
						std::ref(refSolIds),
						std::ref(idMtx),
						std::ref(sols),
						std::ref(belongs[i]),
						std::ref(dis2parents[i]),
						std::cref(fitness),
						env, rnds[i].get()));


				}
				for (auto& thrd : thrds)
					thrd.join();
			}

			mergeBelongInfoMultithread(belongs, dis2parents,
				belong, dis2parent, numThread, rnd);
			
		}



		void calculateNBNiterationThreadTask(
			int & maxId,
			std::mutex& idMtx,
			std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			ofec::Environment* env,
			ofec::Random* rnd) {

			int curId(-1);
			while (true) {
				{
					idMtx.lock();
					curId = maxId++;
					idMtx.unlock();
				}
				if (curId >= sols.size())break;
				auto& curBelong = belong[curId];
				auto& curMinDis = dis2parent[curId];
			
				double curDis = 0;
				for (int idx(0); idx < curId; ++idx) {
					if (sols[curId]->fitness() < sols[idx]->fitness()) {
						curDis = sols[curId]->variableDistance(sols[idx]->variableBase(), env);
						if (curDis < curMinDis) {
							curBelong = idx;
							curMinDis = curDis;
						}
						else if (curDis == curMinDis && rnd->uniform.next() < 0.5) {
							curBelong = idx;
							curMinDis = curDis;
						}
					}
				}


				for (int idx(curId+1); idx < sols.size(); ++idx) {
					if (sols[curId]->fitness() <= sols[idx]->fitness()) {
						curDis = sols[curId]->variableDistance(sols[idx]->variableBase(), env);
						if (curDis < curMinDis) {
							curBelong = idx;
							curMinDis = curDis;
						}
						else if (curDis == curMinDis && rnd->uniform.next() < 0.5) {
							curBelong = idx;
							curMinDis = curDis;
						}
					}
				}
				
				
			}
		}
		


		void calculateNBNiterationMultithread(
			std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			int numThread,
			ofec::Environment* env,
			ofec::Random* rnd) {

			belong.resize(sols.size());
			dis2parent.resize(sols.size());
			std::fill(belong.begin(), belong.end(), -1);
			std::fill(dis2parent.begin(), dis2parent.end(), std::numeric_limits<double>::max());

			int maxId = 0;
			std::mutex idMtx;
			
			std::vector<std::vector<int>> belongs(numThread, belong);
			std::vector<std::vector<double>> dis2parents(numThread, dis2parent);
		
			std::vector<std::shared_ptr<Random>> rnds(numThread);
			for (auto& it : rnds) {
				it.reset(new Random(rnd->uniform.next()));
			}
			{
				std::vector<std::thread> thrds;
				for (size_t i = 0; i < numThread; ++i) {
					double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
					thrds.push_back(std::thread(
						&calculateNBNiterationThreadTask,
						std::ref(maxId),
						std::ref(idMtx),
						std::ref(sols),
						std::ref(belongs[i]),
						std::ref(dis2parents[i]),
						env, rnds[i].get()));


				}
				for (auto& thrd : thrds)
					thrd.join();
			}

			mergeBelongInfoMultithread(belongs, dis2parents,
				belong, dis2parent, numThread, rnd);
		}



		void calculateNBNiterationExperimentOutThreadTask(
			std::ostream& out,int& maxId,int& timeId,
			std::mutex& idMtx,
			std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			ofec::Environment* env,
			ofec::Random* rnd) {
			auto start = std::chrono::system_clock::now();
			int curId(-1);
			bool outFlag = false;
			
			while (true) {
				outFlag = false;
				{
					idMtx.lock();
					curId = maxId++;

					auto end = std::chrono::system_clock::now();
					std::chrono::duration<double> diffSeconds = end - start; // µ¥Î»ÎªÃë
					auto differenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(diffSeconds).count();
					if (differenceInSeconds >= timeId * 1800) {
						++timeId;
						outFlag = true;
					}
					idMtx.unlock();
				}


				if (curId >= sols.size())break;
				if(outFlag){
					
				}

				auto& curBelong = belong[curId];
				auto& curMinDis = dis2parent[curId];

				double curDis = 0;
				for (int idx(0); idx < curId; ++idx) {
					if (sols[curId]->fitness() < sols[idx]->fitness()) {
						curDis = sols[curId]->variableDistance(sols[idx]->variableBase(), env);
						if (curDis < curMinDis) {
							curBelong = idx;
							curMinDis = curDis;
						}
						else if (curDis == curMinDis && rnd->uniform.next() < 0.5) {
							curBelong = idx;
							curMinDis = curDis;
						}
					}
				}


				for (int idx(curId + 1); idx < sols.size(); ++idx) {
					if (sols[curId]->fitness() <= sols[idx]->fitness()) {
						curDis = sols[curId]->variableDistance(sols[idx]->variableBase(), env);
						if (curDis < curMinDis) {
							curBelong = idx;
							curMinDis = curDis;
						}
						else if (curDis == curMinDis && rnd->uniform.next() < 0.5) {
							curBelong = idx;
							curMinDis = curDis;
						}
					}
				}


			}
		}
		



		void calculateNBNiterationMultithreadExperimentOut(
			std::vector<ofec::SolutionBase*>& sols,
			std::ostream& out,
			int numThread,
			ofec::Environment* env,
			ofec::Random* rnd) {
			
			std::vector<std::vector<int>> belongs(numThread);
			std::vector<std::vector<double>> dis2parents(numThread);
			
			for (auto& it : belongs) {
				it.resize(sols.size());
				std::fill(it.begin(), it.end(), -1);
			}
			for (auto& it : dis2parents) {
				it.resize(sols.size());
				std::fill(it.begin(), it.end(), std::numeric_limits<double>::max());
			}

			std::vector<std::shared_ptr<Random>>rnds(numThread);
			for (auto& it : rnds) {
				it.reset(new Random(rnd->uniform.next()));
			}
			
			
		}

	}
}