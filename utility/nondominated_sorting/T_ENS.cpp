#include "T_ENS.h"

namespace NDS {
	void T_ENS(std::vector<std::vector<double>>& Population, int & Noc, std::vector<int>& te_rank, int nSort) {

		const int N = Population.size(); //N = population size
		if (nSort == -1)
			nSort = N;
		const int M = Population[0].size(); //M = number of objectives
		int NoF = -1; //Number of last fronts
		std::vector<int> FrontNo(N, 100000); //front number of each solution
											 /*sort the population in ascending order according to the first
											 objective value, if two solutions have the same value on the first
											 objective value, sort them according to the next objective value*/
		std::vector<int> rank = preSorting(Population, Noc);
		/*the set of fronts(trees)
		Forest[i] means the NO.of the root of the i-th tree
		e.g., Population[Forest[i]] is the root of the i-th tree*/
		std::vector<int> Forest(N, -1);
		/*the NO. of children of each node
		e.g., Population[Children[i][j]] is the j-th child of Population[i]*/
		std::vector<std::vector<int>> Children(N, std::vector<int>(M - 1, -1));
		/*the site of the most left existent child of each node
		e.g., Population[Children[i,LeftChild[i]]] is the most left
		existent child of Population[i]*/
		std::vector<int> LeftChild(N, M - 1);
		/*the NO. of father of each node
		e.g., Population[Father[i]] is the father of Population[i]*/
		std::vector<int> Father(N, -1);
		/*the site of the next existent brother of each node
		e.g., Population[Children[Father[i]][Brother[i]]] is the next
		existent brother of Population[i]*/
		std::vector<int> Brother(N, M - 1);
		//the objective rank of each solution
		std::vector<std::vector<int>> ORank(N, std::vector<int>(M - 1));
		for (size_t j = 0; j < M - 1; ++j) {
			std::vector<double> temp_col(N);
			for (size_t i = 0; i < N; ++i)
				temp_col[i] = Population[i][j + 1];
			std::vector<int> temp_index;
			Noc += OFEC::quick_sort(temp_col, N, temp_index);
			std::vector<int> temp_index2(temp_index);
			temp_index.clear();
			OFEC::quick_sort(temp_index2, N, temp_index);
			for (size_t i = 0; i < N; ++i)
				ORank[i][j] = -temp_index[i];
		}
		for (size_t i = 0; i < N; ++i) {
			std::vector<int> temp_row(M - 1);
			for (size_t j = 0; j < M - 1; ++j)
				temp_row[j] = ORank[i][j];
			std::vector<int> temp_index;
			OFEC::quick_sort(temp_row, M - 1, temp_index);
			for (size_t j = 0; j < M - 1; ++j)
				ORank[i][j] = temp_index[j] + 1;
		}
		//start the non-dominated sorting
		while (([FrontNo]() {int temp_sum(0); for (auto x : FrontNo) if (x < 100000) temp_sum++; return temp_sum; }()) < ([](int a, int b) {return a > b ? b : a; }(nSort, N))) {
			//start sorting on a new front (tree)
			NoF++;
			//let the first solution in the remanining population be the root of the NoF-th tree
			std::vector<int> Remain;
			for (size_t i = 0; i < FrontNo.size(); ++i)
				if (FrontNo[i] == 100000)
					Remain.push_back(i);
			Forest[NoF] = Remain[0];
			FrontNo[Remain[0]] = NoF;
			/*for each solution having not been sorted, compare it with the
			solutions in the NoF-th tree, to see whether it is dominated by
			someone in the tree. And if it is not dominated by anyone, insert
			it into the tree*/
			for (int i = 1; i < Remain.size(); ++i) {
				int p = Remain[i];
				//store the site for pruning of each node during this iteration
				std::vector<int> Pruning(N, 0);
				//compare p with the solutions in the tree, start with the root
				int q = Forest[NoF];
				bool Dominated;
				while (true) {
					//compare p with q
					Dominated = true;
					int m;
					for (m = 0; m < M - 1; ++m) {
						Noc = Noc + 1;
						if (Population[p][ORank[q][m]] < Population[q][ORank[q][m]]) {
							Dominated = false;
							break;
						}
					}
					//p is dominated by q, so p can not be insert into the tree
					if (Dominated)
						break;
					//p is not dominated by q, compare p with the next solution
					else {
						//stores the site for pruning of q
						Pruning[q] = m;
						//if the left most existent child of q has not been pruned, compare p with it
						if (LeftChild[q] <= Pruning[q]) {
							q = Children[q][LeftChild[q]];
							//Noc++;
						}
						//otherwise find the next existent brother of q
						else {
							/*find the next existent brother of q, if it is
							still be pruned, go back to find the next
							existent brother of q's father, until a satisfied
							one is found.*/
							while (Father[q] != -1 && Brother[q] > Pruning[Father[q]]) {
								q = Father[q];
								//Noc++;
							}
							//if a satisfied one is found, compare p with it
							if (Father[q] != -1)
								q = Children[Father[q]][Brother[q]];
							//otherwise means that no solution dominating p is found in the tree
							else
								break;
						}
					}
				}
				//no solution dominating p is found, insert p into the tree
				if (!Dominated) {
					//put p into the current front
					FrontNo[p] = NoF;
					//find the father of p, i.e., the right most node which is not pruned in the tree
					q = Forest[NoF];
					while (Children[q][Pruning[q]] != -1)
						q = Children[q][Pruning[q]];
					//let p be the child of q
					Children[q][Pruning[q]] = p;
					//let q be the father of p
					Father[p] = q;
					//if p is the left most child of q
					if (LeftChild[q] > Pruning[q]) {
						//update the "LeftChild" of q, and the "Brother" of p
						Brother[p] = LeftChild[q];
						LeftChild[q] = Pruning[q];
					}
					//otherwise
					else {
						//find the previous existent brother of p
						int bro = Children[q][LeftChild[q]];
						while (Brother[bro] < Pruning[q]) {
							bro = Children[q][Brother[bro]];
							//Noc++;
						}
						Noc++;
						//update the "Brother" of p and the previous existent brother
						Brother[p] = Brother[bro];
						Brother[bro] = Pruning[q];
					}
				}
			}
		}
		std::vector<int> FrontNo_index;
		OFEC::quick_sort(rank, rank.size(), FrontNo_index);
		for (size_t i = 0; i < FrontNo_index.size(); ++i)
			te_rank[i] = FrontNo[FrontNo_index[i]];
	}
}