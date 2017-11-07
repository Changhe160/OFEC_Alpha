#ifndef DG_NSGAII_H
#define DG_NSGAII_H

#include "NSGAII.h"
#include "DG_NSGAIIindividual.h"
#include "../../../../utility/nondominated_sorting/DominanceGraphSort.h"

namespace OFEC {
	class DG_NSGAII final : public NSGAII<DG_NSGAIIindividual>{
	public:
		DG_NSGAII(param_map &v) : NSGAII(v), m_DG_empty(true){
		}
		void sort();
	protected:
		void evolve_mo();
	protected:
		NDS::dg_graph<double> DG;
		bool m_DG_empty;	
	};
}
#endif // !DG_NSGAII_H

