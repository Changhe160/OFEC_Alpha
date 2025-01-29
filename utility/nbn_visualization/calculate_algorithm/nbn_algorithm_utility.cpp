#include "nbn_algorithm_utility.h"
#include "../../function/custom_function.h"



namespace ofec {
	namespace nbn {
		void mergeBelongInfoThreadTask(
			double seed,
			int from, int to,
			const std::vector<std::vector<int>>& total_belong,
			const std::vector<std::vector<double>>& total_dis2parent,
			std::vector<int>& belong,
			std::vector<double>& dis2parent) {
			Random randNum(seed);
			for (int idx(from); idx < to; ++idx) {

				auto& cur_dis = dis2parent[idx];
				auto& cur_belong = belong[idx];
				for (int idthrd(0); idthrd < total_belong.size(); ++idthrd) {
					if (cur_dis > total_dis2parent[idthrd][idx]) {
						cur_dis = total_dis2parent[idthrd][idx];
						cur_belong = total_belong[idthrd][idx];
					}
					else if (cur_dis == total_dis2parent[idthrd][idx]
						&& randNum.uniform.next() < 0.5) {
						//cur_dis = total_dis2parent[idthrd][idx];
						cur_belong = total_belong[idthrd][idx];
					}
				}
			}
		}




		void mergeBelongInfoMultithread(
			const std::vector<std::vector<int>>& total_belong,
			const std::vector<std::vector<double>>& total_dis2parent,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			int numThread,
			Random* rnd) {
			

			std::vector<std::thread> thrds;
			std::vector<int> tasks;
			UTILITY::assignThreads(belong.size(), numThread, tasks);
			std::pair<int, int> from_to;
			for (size_t i = 0; i < numThread; ++i) {
				from_to.first = tasks[i];
				from_to.second = tasks[i + 1];
				double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
				thrds.push_back(std::thread(
					&mergeBelongInfoThreadTask,randomSeed,
					tasks[i], tasks[i + 1],
					std::cref(total_belong), std::cref(total_dis2parent),
					std::ref(belong), std::ref(dis2parent)
				));
			}
			for (auto& thrd : thrds)
				thrd.join();


		}

	}
}