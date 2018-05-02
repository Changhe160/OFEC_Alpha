#include "best_order_sort.h"
#include <chrono>

int NS::best_order_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement)
{
	std::chrono::time_point<std::chrono::system_clock> Total_start_time;
	int NumComp(0);
	Total_start_time = std::chrono::system_clock::now();

	if (data.empty())
		return 0;
	if (rank.size() != data.size())
		rank.resize(data.size());
	BestOrderSort BOS(data);
	rank = BOS.getRank();	

	int time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
	measurement.first += time;
	measurement.second += NumComp;

	return BOS.getTotalFront();
}

NS::BestOrderSort::BestOrderSort(const std::vector<std::vector<double>>& data) : population(data), mergesort(data) {
	n = population.size();
	m = population[0].size();
	m1 = m;
	best_sort(n, m1);
}

void NS::BestOrderSort::best_sort(int n, int m)
{
	int i, j, total = 0, i2;
	bool dominated;
	Node* head;

	rank.resize(n);
	set.resize(n);
	list.resize(m);
	for (auto& row : list)
		row.resize(n);
	allrank.resize(n);
	for (auto& row : allrank)
		row.resize(m);
	lex_order.resize(n);

	for (j = 0; j<m; j++)
	{
		for (i = 0; i<n; i++)
		{
			allrank[i][j] = i;
		}
	}

	mergesort.setrank(allrank);
	sortingValues();

	for (i = 0; i<n; i++)//n
	{
		for (obj = 0; obj<m; obj++)
		{
			s = allrank[i][obj];
			if (set[s])//s is already ranked
			{
				list[obj][rank[s]].addStart(s);
				continue;
			}
			set[s] = true;
			total++;
			i2 = 0;
			while (true)
			{
				dominated = false;
				head = list[obj][i2].start;
				while (head != nullptr)
				{
					if (dominates(head->data, s))
					{
						dominated = true;
						break;
					}
					head = head->link;
				}
				if (!dominated) //not dominated
				{
					list[obj][i2].addStart(s);
					rank[s] = i2;
					break;
				}
				else //dominated
				{
					if (i2<totalfront)
					{
						i2++;
					}
					else //if it is last node(with highest rank)
					{

						totalfront++;
						rank[s] = totalfront;
						list[obj][rank[s]].addStart(s);
						break;
					}
				}

			}//while(true)
		}
		if (total == n)
		{
			break;
		}
	}
	totalfront++;
}

bool NS::BestOrderSort::dominates(int p1, int p2)
{
	bool equal = true;
	int i;
	for (i = 0; i<m; i++)
	{

		if (population[p1][i] > population[p2][i])
		{
			return false;
		}
		else if (equal && population[p1][i] < population[p2][i])
		{
			equal = false;
		}
	}

	if (equal)//both solutions are equal
		return false;
	else //dominates
		return true;
}

void NS::BestOrderSort::sortingValues()
{
	int j;
	mergesort.sort(0);//Sorting first objectives and get lexicographic order
	allrank = mergesort.getrank();
	for (j = 1; j<n; j++)
	{
		lex_order[allrank[j][0]] = j;
	}
	mergesort.setLexOrder(lex_order);
	for (j = 1; j<m1; j++)
	{
		mergesort.sort_specific(j);
	}
	allrank = mergesort.getrank();
}

//void NS::BestOrderSort::extended_kung_sort_two_dimension()
//{
//	//Initialization
//	int i, j, low, high, middle;
//	double key;
//
//	rank.resize(n);//ranks of solutions
//	allrank.resize(n);
//	for (auto& row : allrank)
//		row.resize(m); //partial lexicographical ordering of x-axis values 
//	std::vector<int> index(n);
//	L.resize(n);
//	for (j = 0; j<m; j++)
//	{
//		for (i = 0; i<n; i++)
//		{
//			allrank[i][j] = i;
//		}
//	}
//	mergesort.setrank(allrank);
//	mergesort.sort(0);//lexicographic sort
//	//b[0] = population[allrank[0][0]][1];//y-value of first rank solution
//	index[0] = allrank[0][0];
//	rank[allrank[0][0]] = 0; //rank of first solution is already found
//	totalfront = 1;
//
//	for (i = 1; i<n; i++)
//	{
//		s = allrank[i][0];//take the solution id
//		key = population[s][1];//the field we would consider
//
//
//							   //-------------Go over all points----------------------//
//		low = 0;
//		high = totalfront - 1;
//
//		while (high >= low)
//		{
//			middle = (low + high) / 2;
//
//			if (key < population[index[middle]][1]) //it has low rank, numerically
//			{
//				high = middle - 1;
//			}
//			else if (key > population[index[middle]][1]) //it has high rank, numerically
//			{
//				low = middle + 1;
//			}
//			else
//			{
//				if (population[index[middle]][0]<population[s][0])
//				{
//					low = middle + 1;
//				}
//				else//first objective was also same
//				{
//					low = rank[index[middle]];
//					break;
//				}
//			}
//		}
//
//		if (low == totalfront)
//		{
//			totalfront = totalfront + 1;
//		}
//		rank[s] = low;
//		index[low] = s;
//	}
//}

//void NS::BestOrderSort::MergeSort::setPopulation(const std::vector<std::vector<double>>& pop) {
//	this->population = pop;
//	this->n = population.size();
//	helper.resize(n);
//}

void NS::BestOrderSort::MergeSort::sort(int obj) { 
	this->obj = obj;
	n = population.size();
	mergesort(0, n - 1);
}

void NS::BestOrderSort::MergeSort::mergesort(int low, int high) {
	// check if low is smaller then high, if not then the array is sorted
	if (low < high)
	{
		// Get the index of the element which is in the middle
		int middle = low + (high - low) / 2;
		// Sort the left side of the array
		mergesort(low, middle);
		// Sort the right side of the array
		mergesort(middle + 1, high);
		// Combine them both
		merge(low, middle, high);
	}
}

void NS::BestOrderSort::MergeSort::merge(int low, int middle, int high)
{
	// Copy both parts into the helper array
	for (int i = low; i <= high; i++)
	{
		helper[i] = rank[i][obj];
	}

	int i = low;
	int j = middle + 1;
	int k = low;
	// Copy the smallest values from either the left or the right side back
	// to the original array
	while (i <= middle && j <= high)
	{
		if (population[helper[i]][obj] < population[helper[j]][obj])
		{
			rank[k][obj] = helper[i];
			i++;
		}
		else if (population[helper[i]][obj] > population[helper[j]][obj])
		{
			rank[k][obj] = helper[j];
			j++;
		}
		else //two values are equal
		{
			check = lexicopgraphic_dominate(helper[i], helper[j]);
			if (check)
			{
				rank[k][obj] = helper[i];
				i++;
			}
			else
			{
				rank[k][obj] = helper[j];
				j++;
			}
		}
		k++;
	}
	while (i <= middle)
	{
		rank[k][obj] = helper[i];
		k++;
		i++;
	}
	while (j <= high)
	{
		rank[k][obj] = helper[j];
		k++;
		j++;
	}

}

void NS::BestOrderSort::MergeSort::sort_specific(int obj)
{
	this->obj = obj;
	n = population.size();
	mergesort_specific(0, n - 1);
}

void NS::BestOrderSort::MergeSort::mergesort_specific(int low, int high)
{
	// check if low is smaller then high, if not then the array is sorted
	if (low < high)
	{
		// Get the index of the element which is in the middle
		int middle = low + (high - low) / 2;
		// Sort the left side of the array
		mergesort_specific(low, middle);
		// Sort the right side of the array
		mergesort_specific(middle + 1, high);
		// Combine them both
		merge_specific(low, middle, high);
	}
}

void NS::BestOrderSort::MergeSort::merge_specific(int low, int middle, int high)
{
	// Copy both parts into the helper array
	for (int i = low; i <= high; i++)
	{
		helper[i] = rank[i][obj];
	}

	int i = low;
	int j = middle + 1;
	int k = low;
	// Copy the smallest values from either the left or the right side back to the original array
	while (i <= middle && j <= high)
	{
		if (population[helper[i]][obj] < population[helper[j]][obj])
		{
			rank[k][obj] = helper[i];
			i++;
		}
		else if (population[helper[i]][obj] > population[helper[j]][obj])
		{
			rank[k][obj] = helper[j];
			j++;
		}
		else //two values are equal
		{
			if (lex_order[helper[i]]<lex_order[helper[j]])
			{
				rank[k][obj] = helper[i];
				i++;
			}
			else
			{
				rank[k][obj] = helper[j];
				j++;
			}
		}
		k++;
	}
	while (i <= middle)
	{
		rank[k][obj] = helper[i];
		k++;
		i++;
	}
	while (j <= high)
	{
		rank[k][obj] = helper[j];
		k++;
		j++;
	}
}

bool NS::BestOrderSort::MergeSort::lexicopgraphic_dominate(int p1, int p2)
{
	int i;

	for (i = 0; i<population[0].size(); i++)
	{
		if (population[p1][i] == population[p2][i])
			continue;
		else if (population[p1][i] < population[p2][i])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}
