#ifndef NBN_MODIFY_SOLUTIONS_TSP_H
#define NBN_MODIFY_SOLUTIONS_TSP_H


#include <vector>
#include <set>
#include "../../../../core/random/newran.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/environment/environment.h"
#include "../../../../instance/problem/combination/travelling_salesman/travelling_salesman.h"


namespace ofec {
	namespace nbn {
		namespace tsp {

			// output data
			void outputTspSolutions(const std::string& saveDir, 
				const std::string& filename, std::vector<SolutionBase*>& solbases);
			

			// sampling
			//void sampleSolsAroundTSPThreadTask(
			//	const ofec::TravellingSalesman::SolutionType& centerSol,
			//	std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
			//	int neighbor_k,
			//	int from, int to,
			//	ofec::Environment* env,
			//	ofec::Random* rnd
			//);


			void sampleSolsAroundTSPMultiThread_kSwap(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			);
			


			void sampleSolsAroundTSPMultiThread_subsequenceRND(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			);


			void sampleSolsAroundTSPMultiThread(
				const ofec::TravellingSalesman::SolutionType& sol,
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int neighbor_k,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			);

			void sampleSolsRandomMultiThread(
				std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
				int numSamples,
				ofec::Environment* env,
				ofec::Random* rnd
			);
			

			// filter same solutions
			void filterUniqueSols(const std::vector<ofec::SolutionBase*>& originSol,
				std::vector<ofec::SolutionBase*>& filteredSols,
				std::vector<int>& toSolId,
				ofec::TravellingSalesman* tsp_pro, ofec::Random* rnd
			);

			void sortEachSolutionX(std::vector<ofec::SolutionBase*>& sols);
			

			

		}
	}
}




#endif