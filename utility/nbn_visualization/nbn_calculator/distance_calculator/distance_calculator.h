#ifndef NBN_DISTANCE_CALCULATOR_H
#define NBN_DISTANCE_CALCULATOR_H

#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <algorithm>
#include <shared_mutex>
#include <chrono>


#include "../../../../core/problem/solution.h"
#include "../../../../core/environment/environment.h"
#include "../../../../utility/function/custom_function.h"
#include "../../../../utility/function/visit_lazy_list.h"


namespace ofec {
	struct NBN_DistanceCalculator {


	protected:

		std::vector<const ofec::SolutionBase*> m_sols;
		std::list<std::array<int, 3>> m_tasks;
		std::vector<double> m_distance;
		std::shared_ptr<ofec::Environment> m_environment = nullptr;
		std::mutex m_mutex;

	protected:


		//void calculateDistacn

		void calculateDistanceTask() {
			std::array<int, 3> cur;
			while (true) {
				{
					std::unique_lock lock(m_mutex);
					if (m_tasks.empty())break;

					cur = m_tasks.front();
					m_tasks.pop_front();
				}

				m_distance[cur[0]] = m_environment->problem()->variableDistance(m_sols[cur[1]]->variableBase(), m_sols[cur[2]]->variableBase());

			}
		}

	public:
		void calculateDistance(
			const std::vector<ofec::SolutionBase*>& sols,
			const std::vector<std::pair<int, int>>& tasks, 
			std::vector<double>& distance, ofec::Environment* env) {
			m_sols.resize(sols.size());
			for (int idx(0); idx < m_sols.size(); ++idx) {
				m_sols[idx] = sols[idx];
			}
			std::array<int, 3> cur;
			for (int idx(0); idx < tasks.size(); ++idx) {
				cur[0] = idx;
				cur[1] = tasks[idx].first;
				cur[2] = tasks[idx].second;
				m_tasks.push_back(cur);
			}
			m_distance.resize(tasks.size());
			m_environment = env->getSharedPtr();


			auto numThread = std::thread::hardware_concurrency();

			std::vector<std::thread> thrds;
			for (size_t i = 0; i < numThread; ++i) {
				thrds.push_back(std::thread(
					&NBN_DistanceCalculator::calculateDistanceTask, this));
			}
			for (auto& thrd : thrds)
				thrd.join();

			distance = std::move(m_distance);
		}

	};
}

#endif