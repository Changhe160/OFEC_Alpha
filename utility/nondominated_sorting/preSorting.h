#ifndef PRE_SORTING_H
#define PRE_SORTING_H

#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"
#include <numeric>

namespace NDS {


	template<typename T>
	bool greater(std::vector<T>& a, std::vector<T>& b, int& Noc) {
		for (size_t i = 0; i < a.size(); i++) {
			Noc++;
			if (a[i] > b[i])
				return true;
			else if (a[i] < b[i])
				return false;
		}
		return true;
	}

	template<typename T>
	int Partition(std::vector<std::vector<T>>& data, std::vector<int>& A, int low, int high, int& Noc)
	{
		int pivot = A[low];
		while (low < high)
		{
			while (low < high && greater(data[A[high]], data[pivot], Noc)) 
				--high;

			A[low] = A[high]; //将比枢轴值小的元素移到左边  

			while (low < high && greater(data[pivot], data[A[low]], Noc))
				++low;

			A[high] = A[low]; //将比枢轴值小的元素移到右边  
		}
		A[low] = pivot;  //将枢轴值元素置于最终位置  
		return low;
	}

	template<typename T>
	void QuickSort(std::vector<std::vector<T>>& data, std::vector<int>& A, int low, int high, int& Noc)
	{
		if (low < high) //递归跳出条件
		{
			//Partition就是上面的划分操作
			int pivot = Partition(data, A, low, high, Noc); //划分

			QuickSort(data, A, low, pivot - 1, Noc); //左半部分递归

			QuickSort(data, A, pivot + 1, high, Noc); //右半部分递归
		}
	}

	template<typename T>
	std::vector<int> preSorting(std::vector<std::vector<T>>& data, int& Noc, bool ascending = true) {

		std::vector<int> index(data.size());
		iota(index.begin(), index.end(), 0);
		QuickSort(data, index, 0, index.size() - 1, Noc);

		if (!ascending) {
			std::vector<int> new_index(index.size());
			for (size_t i = 0; i < data.size(); i++)
				new_index[i] = index[index.size() - 1 - i];
			index = new_index;
		}
		std::vector<std::vector<T>> new_data(data.size());
		for (size_t i = 0; i < data.size(); i++)
			new_data[i] = data[index[i]];
		data = new_data;

		return index;
	}
}

#endif // !PRE_SORTING_H

