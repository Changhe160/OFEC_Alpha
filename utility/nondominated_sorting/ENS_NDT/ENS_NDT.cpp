#include "ENS_NDT.h"

namespace NS {
	int ENS_NDT_sort(const std::vector<std::vector<double>>& individuals, std::vector<int>& rank, std::pair<int, int>& measurement)
	{
		std::chrono::time_point<std::chrono::system_clock> Total_start_time;
		int NumComp(0);
		Total_start_time = std::chrono::system_clock::now();

		if (individuals.size() == 0)
			return 0;
		int num_fro = ENS_NDT::NondominatedSort(individuals, individuals[0].size(), NumComp, rank);

		int time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
		measurement.first += time;
		measurement.second += NumComp;

		return num_fro;
	}

	int ENS_NDT::NondominatedSort(const std::vector<std::vector<double>>& P, int k, int& NumComp, std::vector<int>& rank)
	{
		// Create a copy of the population and sort it in reverse lexicographic order
		std::vector<std::vector<double>> S(P.size());
		std::vector<int> temp_index;
		NumComp += NS::quick_sort(P, temp_index, k - 1);
		for (int i = 0; i < P.size(); ++i)
			S[i] = P[temp_index[i]];

		// Generate the prebalanced splits for the NDTree
		std::vector<std::vector<double>> points = S;
		std::shared_ptr<NDSplit> splits = NDSplit::GenerateSplits(points, k - 1, NumComp, 2);

		// Create the front set "F" and the NDTree front set "NDT" and add the first solution to both
		std::vector<std::vector<int>> F({{temp_index[0]}});
		std::vector<std::unique_ptr<NDTree>> NDT;
		NDT.push_back(std::unique_ptr<NDTree>(new NDTree(splits)));
		NDT[0]->Insert(&S[0], NumComp);

		// Do the non dominated sorting
		int j = 0;
		for (int i = 1; i < S.size(); ++i)
		{
			if (!FitnessEquals(S[i], S[i - 1], k, NumComp))
			{
				j = FrontIndexBinarySearch(&S[i], NDT, NumComp);
				if (j == F.size())
				{
					F.push_back(std::vector<int>());
					NDT.push_back(std::unique_ptr<NDTree>(new NDTree(splits)));
				}
				NDT[j]->Insert(&S[i], NumComp);
			}
			F[j].push_back(temp_index[i]);
		}
		rank.resize(P.size());
		for (int rank_num = 0; rank_num < F.size(); ++rank_num)
			for (auto idx : F[rank_num])
				rank[idx] = rank_num;

		//for (int i = 0; i < NDT.size(); ++i)
		//	delete NDT[i];

		return F.size();
	}
	int ENS_NDT::FrontIndexBinarySearch(std::vector<double>* s, std::vector<std::unique_ptr<NDTree>>& NDT, int& NumComp)
	{
		int i1 = 0;
		int i2 = NDT.size();
		while (i1 != i2)
		{
			int i = i1 + (i2 - i1) / 2;
			if (NDT[i]->Dominates(s, NumComp))
				i1 = i + 1;
			else
				i2 = i;
		}
		return i1;
	}
	bool ENS_NDT::FitnessEquals(std::vector<double>& a, std::vector<double>& b, int k, int& NumComp)
	{
		std::chrono::time_point<std::chrono::system_clock> start_time;
		for (int i = 0; i < k; ++i) {
			NumComp++;
			if (a[i] != b[i]) {
				return false;
			}
		}
		return true;
	}
}