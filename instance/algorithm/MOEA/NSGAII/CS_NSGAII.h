#ifndef CS_NSGAII_H
#define CS_NSGAII_H

#include "NSGAII.h"
#include "../../../../utility/nondominated_sorting/CornerSort.h"

namespace OFEC {
	class CS_NSGAII final : public NSGAII<> {
	public:
		CS_NSGAII(param_map &v) : NSGAII(v){}
		void sort();
	};
}

#endif // !CS_NSGAII_H

