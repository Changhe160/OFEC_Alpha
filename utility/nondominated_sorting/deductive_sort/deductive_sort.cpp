#include "deductive_sort.h"
#include <string.h>
#include "../../functional.h"

namespace NS {
	void deductive_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement){

		std::chrono::time_point<std::chrono::system_clock> Total_start_time;
		std::chrono::microseconds Total_time_cost;
		Total_time_cost = Total_time_cost.zero();
		int NumComp(0);
		Total_start_time = std::chrono::system_clock::now();

		const int N = data.size(); // size of data
		if (N == 0) return;
		const int M = data.front().size(); // number of objective
		int x = 0; // front number
		int f = 0; // number sorted
		bool* F = new bool[N]; // ranked flag
		memset(F, false, N);
		bool* D = new bool[N]; // dominated flag
		while (f < N) {
			memset(D, false, N);
			for (int i = 0; i < N; ++i) {
				if (!D[i] && !F[i]) {
					for (int j = i + 1; j < N; ++j) {
						if (!D[j] && !F[j]) {
							std::pair<OFEC::dominationship, int> result = OFEC::objective_compare(data[j], data[i], OFEC::optimization_mode::Minimization);
							NumComp += result.second;
							OFEC::dominationship d = result.first;
							if (d == OFEC::dominationship::Dominated)
								D[j] = true;
							else if (d == OFEC::dominationship::Dominating) {
								D[i] = true;
								break;
							}
						}
					}
					if (!D[i]) {
						rank[i] = x;
						f++;
						F[i] = true;
					}
				}
			}
			x++;
		}
		delete[] D;
		delete[] F;

		int time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
		measurement.first += time;
		measurement.second += NumComp;

	}
}