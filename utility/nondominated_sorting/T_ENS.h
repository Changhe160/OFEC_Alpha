#ifndef T_ENS_H
#define T_ENS_H

#include "../functional.h"
#include "preSorting.h"

namespace NDS {

	// return FrontNo
	void T_ENS(std::vector<std::vector<double>>& Population, int& Noc, std::vector<int>& te_rank, int nSort = -1); 
}

#endif // !T_ENS_H

