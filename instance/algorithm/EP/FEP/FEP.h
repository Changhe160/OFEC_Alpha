#ifndef OFEC_FEP_H
#define OFEC_FEP_H

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class FEP_pop : public EP::population<> {
	public:
		FEP_pop(size_t size_pop);
	protected:
		void mutate() override;
	};

	class FEP : public algorithm {
	public:
		FEP(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		FEP_pop m_pop;
	};
}

#endif // !OFEC_FEP_H
