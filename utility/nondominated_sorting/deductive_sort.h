#ifndef DEDUCTIVESORT_H
#define DEDUCTIVESORT_H

/*K.M.Clymont and E.Keedwell, ¡°Deductive sort and climbing sort :
new methods for non - dominated sorting, ¡± Evolutionary Computation,
vol. 20, no. 1, pp. 1¨C26, 2012.*/

#include <vector>
#include "../../core/definition.h"
#include "../../utility/functional.h"
#include <cstring>

namespace OFEC {
	namespace NS {
		template<typename ObjectiveType>
		void deductive_sort(const std::vector<std::vector<ObjectiveType>*>& data, std::vector<int>& rank, const std::vector<optimization_mode>& opt_mode) {
			const int N = data.size(); // size of data
			if (N == 0) return;
			if (rank.size() != N) rank.resize(N);
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
								dominationship d = objective_compare<ObjectiveType>(*data[j], *data[i], opt_mode);
								if (d == dominationship::Dominated)
									D[j] = true;
								else if (d == dominationship::Dominating) {
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
		}
	}
}

#endif // !DEDUCTIVESORT_H
