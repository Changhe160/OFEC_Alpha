#ifndef PRE_SORTING_H
#define PRE_SORTING_H

#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"
#include <numeric>

namespace NSD {
	template<typename T>
	void preSorting(vector<vector<T>>& data, bool ascending = true) {

		vector<int> index(data.size());
		iota(index.begin(), index.end(), 0);
		QuickSort(data, index, 0, index.size()-1);

		vector<vector<T>> newdata(data.size());
		if (ascending)
			for (int i = 0; i < data.size(); i++)
				newdata[i] = data[index[i]];
		else
			for (int i = 0; i < data.size(); i++)
				newdata[i] = data[index[data.size() - 1 - i]];
		data = newdata;
	}

	template<typename T>
	bool greater(vector<T>& a, vector<T>& b) {
		bool m(true);
		for (int i = 0; i < a.size(); i++) {
			if (a[i] > b[i]) {
				break;
			}
			else if (a[i] < b[i]) {
				m = false;
				break;
			}
		}
		return m;
	}

	template<typename T>
	int Partition(vector<vector<T>>& data, vector<int>& A, int low, int high)
	{
		int pivot = A[low];
		while (low < high)
		{
			while (low < high && greater(data[A[high]], data[pivot])) 
				--high;

			A[low] = A[high]; //将比枢轴值小的元素移到左边  

			while (low < high && greater(data[pivot], data[A[low]]))
				++low;

			A[high] = A[low]; //将比枢轴值小的元素移到右边  
		}
		A[low] = pivot;  //将枢轴值元素置于最终位置  
		return low;
	}

	template<typename T>
	void QuickSort(vector<vector<T>>& data, vector<int>& A, int low, int high)
	{
		if (low < high) //递归跳出条件
		{
			//Partition就是上面的划分操作
			int pivot = Partition(data, A, low, high); //划分

			QuickSort(data, A, low, pivot - 1); //左半部分递归

			QuickSort(data, A, pivot + 1, high); //右半部分递归
		}
	}
}

#endif // !PRE_SORTING_H

