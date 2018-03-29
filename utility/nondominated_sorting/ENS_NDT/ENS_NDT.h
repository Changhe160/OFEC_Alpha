#ifndef ENS_NDT_H
#define ENS_NDT_H

/*
A New Algorithm Using the Non-Dominated Tree to Improve Non-Dominated Sorting
January 2017Evolutionary Computation
DOI10.1162/EVCO_a_00204
*/

#include "NDTree.h"

using namespace ENS_NDT;

namespace NDS {
	class ENS_NDT {
	public:
		static std::vector<int> Sort(const std::vector<std::vector<double>>& individuals, std::pair<int, int>& measurement);
		static std::vector<int> NondominatedSort(const std::vector<std::vector<double>>& P, int k, int& NumComp);
	private:
		static int FrontIndexBinarySearch(std::vector<double>* s, std::vector<NDTree*>& NDT, int& NumComp);
		static bool FitnessEquals(std::vector<double>& a, std::vector<double>& b, int k, int& NumComp);
	};
}

#endif // !ENS_NDT_H

