#ifndef OFEC_CEP_H
#define OFEC_CEP_H

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class CEP : public algorithm {   
	public:
		CEP(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		EP::population<> m_pop;
	};
}

#endif // !OFEC_CEP_H
