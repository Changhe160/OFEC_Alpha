#ifndef OFEC_SIMULATED_ANNEALING_H
#define OFEC_SIMULATED_ANNEALING_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/solution.h"

namespace OFEC {
	class simulated_annealing : public algorithm {
	public:
		simulated_annealing(param_map& v);
		simulated_annealing(size_t k_max);
		void initialize();
		void record();
	protected:
		void run_();
		virtual real temperature(real val);
		virtual void neighbour(const solution<>& s, solution<>& s_new);
		virtual real P(const solution<>& s, const solution<>& s_new, real T);
	protected:
		size_t m_k_max;
		std::unique_ptr<solution<>> m_s;
		std::unique_ptr<solution<>> m_s_new;
	};
}

#endif // !OFEC_SIMULATED_ANNEALING_H

