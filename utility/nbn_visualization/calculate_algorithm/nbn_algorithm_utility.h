#ifndef  NBN_ALGORITHM_UTILITY
#define  NBN_ALGORITHM_UTILITY

#include <vector>
#include "../../../core/random/newran.h"

namespace ofec {
	namespace nbn {
		void mergeBelongInfoThreadTask(
			double seed,
			int from, int to,
			const std::vector<std::vector<int>>& total_belong,
			const std::vector<std::vector<double>>& total_dis2parent,
			std::vector<int>& belong,
			std::vector<double>& dis2parent);

		void mergeBelongInfoMultithread(
			const std::vector<std::vector<int>>& total_belong,
			const std::vector<std::vector<double>>& total_dis2parent,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			int numThread,
			Random* rnd);
		
	}
}


#endif // ! NBN_ALGORITHM_UTILITY
