#include "register_algorithm.h"
#include "include_algorithm.h"

namespace OFEC {
	void register_algorithm() {
		REGISTER(algorithm, NSGAII_SBX, "NSGAII-SBX", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, NSGAIII_SBX, "NSGAIII-SBX", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, NSGAII_DE, "NSGAII-DE", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, GrEA, "GrEA", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, CDGMOEA, "CDG-MOEA", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, MOEAD_SBX, "MOEAD-SBX", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		REGISTER(algorithm, MOEAD_DE, "MOEAD-DE", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));

		REGISTER(algorithm, CMA_ES, "CMA-ES", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, CEP, "CEP", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, FEP, "FEP", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, DE_rand_1, "DE-rand-1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, DE_best_2, "DE-best-2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, JADE, "JADE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, jDE, "jDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, SaDE, "SaDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, SPSO07, "SPSO07", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));
		REGISTER(algorithm, SPSO11, "SPSO11", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));

		REGISTER(algorithm, CRDE, "CRDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::GOP,problem_tag::ConOP }));
		REGISTER(algorithm, NCDE, "NCDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::GOP,problem_tag::ConOP }));
		REGISTER(algorithm, NSDE, "NSDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::GOP,problem_tag::ConOP }));
		REGISTER(algorithm, DE_nrand_1, "DE-nrand-1", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::GOP,problem_tag::ConOP }));

		REGISTER(algorithm, DynDE, "DynDE", std::set<problem_tag>({ problem_tag::DOP,problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));

		REGISTER(algorithm, simulated_annealing, "SA", std::set<problem_tag>({ problem_tag::ConOP }));

	}
}
