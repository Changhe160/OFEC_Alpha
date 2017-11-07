#include "T_ENS_NSGAII.h"

namespace OFEC {
	void T_ENS_NSGAII::sort() {
		const int N = m_offspring.size();
		const int M = global::ms_global->m_problem->objective_size();
		std::vector<std::vector<double>> data(N, std::vector<double>(M));
		for (size_t i = 0; i < N; ++i)
			for (size_t j = 0; j < M; ++j)
				data[i][j] = m_offspring[i].get_objective()[j];
		std::vector<int> te_rank(N);
		NDS::T_ENS(data, m_objcomp, te_rank);
		for (size_t i = 0; i < N; ++i)
			m_offspring[i].set_rank(te_rank[i]);
	}
}
