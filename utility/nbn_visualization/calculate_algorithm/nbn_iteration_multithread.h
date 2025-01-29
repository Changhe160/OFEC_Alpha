#ifndef OFEC_NBN_ITERATION_MULTITHREAD_H
#define OFEC_NBN_ITERATION_MULTITHREAD_H

#include "../../../core/random/newran.h"
#include "../../../core/environment/environment.h"



namespace ofec {
	namespace nbn {
		


		void updateNearsetBetterSolution(
			int curSolId,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd);
		


		void updateNearsetBetterSolutions(
			const std::vector<int>& solIds,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd);


		void calculateNBNiterationMultithread(
			std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			int numThread,
			ofec::Environment* env,
			ofec::Random* rnd);
		

		void calculateNBNiterationMultithreadExperimentOut(
			std::vector<ofec::SolutionBase*>& sols,
			std::ostream& out,
			int numThread,
			ofec::Environment* env,
			ofec::Random* rnd);

		

	}
}




#endif