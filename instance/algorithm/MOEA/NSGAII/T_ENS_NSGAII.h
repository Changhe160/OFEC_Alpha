#ifndef T_ENS_NSGAII_H
#define T_ENS_NSGAII_H
#include "NSGAII.h"
#include "../../../../utility/nondominated_sorting/T_ENS.h"

namespace OFEC {
	class T_ENS_NSGAII final : public NSGAII<> {
	public:
		T_ENS_NSGAII(param_map &v) : NSGAII(v) {}
		void sort();
	};
}

#endif // !T_ENS_NSGAII_H

