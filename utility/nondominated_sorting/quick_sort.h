#ifndef NDS_QUICK_SORT_H
#define NDS_QUICK_SORT_H

#include <vector>
#include <thread>
#include <memory> 
namespace NS {

	template<typename T>
	bool greater(const std::vector<T>& a, const std::vector<T>& b, const int obj_idx, int& Noc) {
		std::size_t obj_num = a.size();
		for (std::size_t i = 0; i < obj_num; ++i) {
			int idx = (i + obj_idx) % obj_num;
			Noc++;
			if (a[idx] > b[idx])
				return true;
			else if (a[idx] < b[idx])
				return false;
		}
		return true;
	}

	template<typename T>
	int partition(const std::vector<std::vector<T>>& data, std::vector<int>& A, const int obj_idx, int low, int high, int& Noc)
	{
		int pivot = A[low];
		while (low < high)
		{
			while (low < high && greater(data[A[high]], data[pivot], obj_idx, Noc))
				--high;

			A[low] = A[high]; //将比枢轴值小的元素移到左边  

			while (low < high && greater(data[pivot], data[A[low]], obj_idx, Noc))
				++low;

			A[high] = A[low]; //将比枢轴值小的元素移到右边  
		}
		A[low] = pivot;  //将枢轴值元素置于最终位置  
		return low;
	}

	template<typename T>
	void QuickSort(const std::vector<std::vector<T>>& data, std::vector<int>& A, const int obj_idx, int low, int high, int& Noc)
	{
		if (low < high) //递归跳出条件
		{
			//partition就是上面的划分操作
			int pivot = partition(data, A, obj_idx, low, high, Noc); //划分

			QuickSort(data, A, obj_idx, low, pivot - 1, Noc); //左半部分递归

			QuickSort(data, A, obj_idx, pivot + 1, high, Noc); //右半部分递归
		}
	}

	template<typename T>
	int quick_sort(const std::vector<std::vector<T>>& data, std::vector<int>& index, const int obj_idx, int low = -1, int high = - 1, bool ascending = true) {
		if (low == -1) {
			low = 0;
			high = data.size() - 1;
		}
		const int N = high - low + 1;
		if (index.size() == 0 || index.size() != N)		index.resize(N);

		std::vector<int> temp_index(data.size());
		for (auto i = temp_index.begin(); i != temp_index.end(); ++i) *i = i - temp_index.begin();
		int Noc(0);
		QuickSort(data, temp_index, obj_idx, low, high, Noc);
		for (int i = 0; i < N; ++i)
			index[i] = temp_index[i + low];
		if (!ascending) {
			for (std::size_t i = 0; i < N / 2; i++)
				std::swap(index[i], index[N - 1 - i]);
		}
		return Noc;
	}

	template<class T>
	bool compare(const T &d1, const T &d2, bool ascending = true) {
		if (ascending) {
			if (d1<d2) return true;
			else return false;
		}
		else {
			if (d1<d2) return false;
			else return true;
		}
	}

	template<class T>
	bool compare(T *d1, T *d2, bool ascending = true) {
		if (ascending) {
			if (*d1<*d2) return true;
			else return false;
		}
		else {
			if (*d1<*d2) return false;
			else return true;
		}
	}

	template<class T>
	void quick_sort(const T &data, int size, std::vector<int>& index, bool ascending = true, int low = 0, int up = -1, int num = -1,bool first=true) {
		//sort data from small to large, and put the order in index
		//size: the size of data  
		//low, up : the range of data to be sorted
		//num : the max/min number of data within low and up 
		static thread_local std::unique_ptr<int> lb;
		static thread_local std::unique_ptr<std::vector<bool>> flag;
		if (first)	{
			if (up == -1) up = size - 1;
			if (num == -1) num = size;
			flag.reset(new std::vector<bool>(num, false));
			lb.reset(new int(low));
			if (index.size() == 0 || index.size() != size)		
				index.resize(size);
			for (auto i = index.begin(); i != index.end(); ++i) 
				*i = i - index.begin();
		}
		if (low >= up) return;
		int i = 0;
		for (; i<num; i++) {
			if ((*flag.get())[i] == false)	
				break;
		}
		if (i == num) return;
		int left = low + 1;
		int right = up;
		int pivot = low;

		while (left<right) {
			while (compare(data[index[left]], data[index[pivot]], ascending) && left<right)
				left++;
			while (!compare(data[index[right]], data[index[pivot]], ascending) && left<right)
				right--;
			if (left<right) {
				int t = index[left];
				index[left] = index[right];
				index[right] = t;
			}
		}
		while (!compare(data[index[left]], data[index[pivot]], ascending) && left>pivot)
			left--;
		if (compare(data[index[left]], data[index[pivot]], ascending)) {
			int t = index[left];
			index[left] = index[pivot];
			index[pivot] = t;
			if (left - *lb<num)
				(*flag.get())[left - *lb] = true;
		}
		else  {
			if (pivot - *lb<num)
				(*flag.get())[pivot - *lb] = true;
		}
		i = 0;
		for (; i<num; i++) {
			if ((*flag.get())[i] == false)		break;
		}
		if (i == num) return;

		pivot = left;
		quick_sort(data, pivot - low, index, ascending, low, pivot - 1, num, false);
		quick_sort(data, up - pivot, index, ascending, pivot + 1, up, num, false);
	}

}

#endif // !NDS_QUICK_SORT_H

