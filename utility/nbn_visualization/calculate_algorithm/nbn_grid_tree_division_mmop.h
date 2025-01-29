

#ifndef NBN_GRID_DIVISION_TREE_MMOP_H
#define NBN_GRID_DIVISION_TREE_MMOP_H

#include <vector>
#include <memory>
#include <queue>

#include "nbn_grid_tree_division.h"
#include "nbn_grid_tree_division_allSize.h"

namespace ofec {
	class NBN_GridTreeDivisionMMOP : public NBN_GridTreeDivision_allSize {

	protected:

		int m_dimemsion_division = 0;


	public:
		//	using SolutionType = SolBase;
		//	using SolContinousType = Solution<>;


		void setDimensioinDivNumber(int num) {
			m_dimemsion_division = num;
		}
	
		virtual void generateSols()override;
		virtual void updateFitness()override;


	 //   double mapFitness(ofec::SolBase& sol) {
		//	return sol->fitness();
		//}

	};
}

#endif