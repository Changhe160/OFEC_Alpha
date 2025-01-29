/********* Begin Register Information **********
{
    "description": "",
    "identifier": "HillVallEA",
    "name": "HillVallEA",
    "tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

/**********************************************************************************************
@inproceedings{maree2018real,
  address = {New York, NY, USA},
  pages = {857--864},
  year = {2018},
  author = {S. C. Maree and T. Alderliesten and D. Thierens and P. A. N. Bosman},
  series = {GECCO '18},
  location = {Kyoto, Japan},
  publisher = {Association for Computing Machinery},
  title = {Real-valued evolutionary multi-modal optimization driven by hill-valley clustering},
  booktitle = {Proceedings of the Genetic and Evolutionary Computation Conference}
}
***********************************************************************************************/

#ifndef OFEC_HILL_VALLEA_H
#define OFEC_HILL_VALLEA_H

#include "../../../../../../core/algorithm/algorithm.h"
#include "../../../../../../utility/linear_algebra/matrix.h"
#include "../../../../../../core/problem/solution.h"

namespace ofec {
	class HillVallEA : virtual public Algorithm {
		OFEC_CONCRETE_INSTANCE(HillVallEA)
	protected:
		size_t m_N, m_N_c;
		Real m_tau, m_TOL;
		std::list<std::shared_ptr<Solution<>>> m_E;

		int m_restartTime = 0;
		int m_curpopid = 0;

		void addInputParameters() {}
		void initialize_(Environment *env) override;
		void run_(Environment *env) override;
		std::shared_ptr<Solution<>> coreSearch(
			const std::vector<std::shared_ptr<Solution<>>> &sols,
			size_t pop_size, Environment *env, Random *rnd);
		std::vector<std::shared_ptr<Solution<>>> uniformlySample(
			size_t num, Environment *env, Random *rnd);

	public:
		static std::vector<std::shared_ptr<Solution<>>> truncationSelection(
			const std::vector<std::shared_ptr<Solution<>>> &sols, Real tau, Environment *env);
		static std::vector<std::vector<std::shared_ptr<Solution<>>>> clustering(
			const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env);
		static bool test(const Solution<>& s1, const Solution<> &s2, size_t num, 
			Environment *env);
		static std::shared_ptr<Solution<>> best(
			const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env);
		static bool duplicate(const Solution<> &s, 
			const std::list<std::shared_ptr<Solution<>>> &sols, Environment *env);
		static bool distinct(const Solution<> &s, 
			const std::list<std::shared_ptr<Solution<>>> &sols, Environment *env);
		static bool betterThanAll(const Solution<> &s,
			const std::list<std::shared_ptr<Solution<>>> &sols, Real TOL, Environment *env);
	};
}

#endif // ! OFEC_HILL_VALLEA_H
