#ifndef OFEC_DATUM_OFFLINE_NBN_POP_DATA_H
#define OFEC_DATUM_OFFLINE_NBN_POP_DATA_H

#include <vector>
#include "../../../core/problem/solution.h"

namespace ofec::datum {
	struct NBN_PopDataIndex {
		bool working = false;
		std::vector<int> m_popIds;
		std::vector<int> m_totalIds;
		
	};
	extern NBN_PopDataIndex g_curPopIds;
}

#endif // !OFEC_DATUM_MULTI_POP_H
