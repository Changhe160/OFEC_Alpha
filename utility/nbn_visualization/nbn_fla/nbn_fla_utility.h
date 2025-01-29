#ifndef NBN_FLA_UTILITY_H
#define NBN_FLA_UTILITY_H


#include <vector>
#include <set>
#include "../../../core/random/newran.h"
#include "../../../core/problem/solution.h"


#include "nbn_info.h"

namespace ofec {
	namespace nbn	{


	
		double analyzeFitnesslandscapeDifference(
			const std::vector<int>& belong,
			const std::vector<double>& fitness);

		bool judgeNBN(
			const std::vector<int>& belong,
			const std::vector<double>& fitness);


		//void calBasinSize(std::vector<int>& belong,
		//	std::vector<double>& basin);

		void calBasins(const std::vector<int>& belong,
			const std::vector<double>& dis2parent,
			double m_basin_radius_cut,
			std::vector<int>& optIds,
			std::vector<int>& belongBid,
			std::vector<std::vector<int>>& basin_idxs);


		void getBasin(const std::vector<int>& belong,
			int peakId, std::vector<int>& curBasin);



		void getSubBasin(const std::vector<std::vector<int>>& sons,
			int peakId, std::vector<int>& curBasin);


		void getSubBasin(const std::vector<std::set<int>>& sons,
			int peakId, std::vector<int>& curBasin);


		void getDirectBasin(
			const NBNinfo& nbnInfo,
			std::vector<std::vector<int>>& basin_idxs
		);

		void getBasinSize(
			const NBNinfo& nbnInfo,
			std::vector<std::vector<int>>& sons,
			std::vector<int>& basinSizes);


		void getBasinTreeDistanceRatio(
			const NBNinfo& nbnInfo,
			std::vector<Real>& distanceRatio
		);


		void calculateBasins(
			std::vector<std::vector<int>>& basins,
			const NBNinfo& nbnInfo,
			const std::vector<std::vector<int>>& selectedCenters
		);
		void calculateBasins(
			std::vector<std::vector<int>>& basins,
			const NBNinfo& nbnInfo,
			const std::vector<int>& selectedCenter
		);


		void calculateBasinBestIds(
			const NBNinfo& nbnInfo,
			const std::vector<std::vector<int>>& basins,
			std::vector<int>& basinBestIds
		);


		void divideBasinByDistanceBasinSize(
			const NBNinfo& nbnInfo,
			double distanceThreadhold,
			int basinSizeThreadhold,
			std::vector<std::vector<int>>& basins);





		// combinatorial experiments related methods
		//Ruggedness indicator
		double calculateNBDstd(
			const NBNinfo& nbnInfo);


		double calculateNeutralityArea(
			const NBNinfo& nbnInfo,
			double ruggness_eps = 0.0001);



		int calculateMaxNeutralBasins(
			std::vector<int>& neurtual_size,
			const NBNinfo& nbnInfo,
			double neuralityRatio=1e-4);


		// select the best BoAs 
		// if NBD>= distanceRatio* meanNBD  &&  fitness>= (1.0- bestRatio) 
		void getOptimumIds(
			std::vector<int>& optIds,
			const NBNinfo& nbnInfo,
			double bestRatio = 0.1, double distanceRatio = 3.0);
		

		// if NBD>= distanceRatio
		void getOptimumIdsByDistance(
			std::vector<int>& optIds,
			const NBNinfo& nbnInfo, double distanceRatio = 3.0);

		

		// max NBD of  alll optima
		double calculateMaxNBDofOptimum(
			const NBNinfo& nbnInfo,
			const std::vector<int>& optIds);

		
		double basinSizesSTD(const std::vector<std::vector<int>>& basins);
		

		double maxNeutralitySons(const NBNinfo& nbnInfo,double neuralityRatio = 1e-4);

		

		double getNBDvalue(const NBNinfo& nbnInfo,
			double bestRatio = 0.1);
		
		double getMaxNBD(const NBNinfo& nbnInfo);
		double getMaxNBD(const NBNinfo& nbnInfo, int& maxId);
		double calculateMeanNBD(const NBNinfo& nbnInfo);
		double calculateMaxNBD(const NBNinfo& nbnInfo);


		// sum_{x in X }  f(b(x))< f(x)

		//  sum_{x in X}  f_1(b_2(x)) >= f_1(b_2(x)),  f_2(b_1(x))>= f_2(b_1(x))
		double diffNBNRatio(const NBNinfo& cur, const NBNinfo& other);
		
		
	//	double calculateMaxNBD(const NBNinfo& nbnInfo, const std::vector<int>& ids);


		void pathToRoot(int curId, const NBNinfo& nbnInfo, std::vector<int>& path);
		
		double pathDistance(const NBNinfo& nbnInfo,const std::vector<int>& path);

		void filterBestSolutions(std::vector<int>& solIds,const NBNinfo& nbnInfo,
			double fitnessThread = 0.99,
		double disThread = 30);

		
		

		void getBestSolIds(const NBNinfo& nbnInfo, std::vector<int>& solIds);
		void getSecondFurtherSolIds(const NBNinfo& nbnInfo, std::vector<int>& solIds);
		

		// algorithm related

		//void getAlgorithmTraitOpts();
		
		
	}

}

#endif