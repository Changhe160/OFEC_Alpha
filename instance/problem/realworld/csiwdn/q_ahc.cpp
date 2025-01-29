#include "q_ahc.h"
#include <iostream>

namespace ofec::csiwdn {
	void Q_AHC::initialize(const std::vector<std::vector<bool>>& adjmatrix) {
		m_num_cluster = adjmatrix.size();
		m_cluster.resize(adjmatrix.size());
		m_a.resize(adjmatrix.size());
		for (size_t i = 0; i < adjmatrix.size() * adjmatrix.size(); i++) {
			m_pos_in_seq.push_back(-1);
		}
		for (size_t i = 0; i < adjmatrix.size(); i++) {
			m_cluster[i].cluster_id = i;
			m_cluster[i].member.push_back(i);
			int n = 0;
			for (int j = 0; j < adjmatrix[i].size(); j++) {
				if (adjmatrix[i][j] == true)
					n++;
			}
			m_a[i] = (double)n / (2 * adjmatrix.size());
		}
		for (int i = 0; i < adjmatrix.size(); i++) {
			for (int j = 0; j < adjmatrix.size(); j++) {
				if (adjmatrix[i][j] == true) {
					m_Q.push_back(std::make_pair(std::make_pair(i, j), ( 1+1.0 / (2 * adjmatrix.size())) - m_a[i] * m_a[j]));
				}
				else {
					if (i == j)
						m_Q.push_back(std::make_pair(std::make_pair(i, j), -1.0));
					else
						m_Q.push_back(std::make_pair(std::make_pair(i, j), 1.0));
				}
			}
		}
	}

	void Q_AHC::clear() {
		m_Q.clear();
		m_seq_of_heap.clear();
		m_pos_in_seq.clear();
		m_a.clear();
		m_num_cluster = 0;
		m_cluster.clear();
	}

	void Q_AHC::build_heap() {
		for (size_t i = 0; i < m_Q.size(); i++) {
			m_pos_in_seq[i] = m_seq_of_heap.size();
			m_seq_of_heap.push_back(i);
			int child = m_pos_in_seq[i];
			int parent = (child - 1) / 2;
			while (child > 0) {
				if (m_Q[m_seq_of_heap[parent]].second >= m_Q[m_seq_of_heap[child]].second)
					break;
				auto temp = m_seq_of_heap[parent];
				m_seq_of_heap[parent] = m_seq_of_heap[child];
				m_seq_of_heap[child] = temp;
				m_pos_in_seq[m_seq_of_heap[child]] = child;
				m_pos_in_seq[m_seq_of_heap[parent]] = parent;
				child = parent;
				parent = (child - 1) / 2;
			}
		}
	}

	void Q_AHC::erase_heap(size_t id) {
		m_seq_of_heap[m_pos_in_seq[id]] = m_seq_of_heap.back();
		m_pos_in_seq[m_seq_of_heap.back()] = m_pos_in_seq[id];
		m_seq_of_heap.pop_back();
		int parent = m_pos_in_seq[id];
		int child = 2 * parent + 1;
		m_pos_in_seq[id] = -1;
		while (child < m_seq_of_heap.size()) {
			if (static_cast<size_t>(child) + 1 < m_seq_of_heap.size() && m_Q[m_seq_of_heap[child]].second < m_Q[m_seq_of_heap[child+1]].second)
				child++;
			if (m_Q[m_seq_of_heap[parent]].second >= m_Q[m_seq_of_heap[child]].second)
				break;
			auto temp = m_seq_of_heap[parent];
			m_seq_of_heap[parent] = m_seq_of_heap[child];
			m_seq_of_heap[child] = temp;
			m_pos_in_seq[m_seq_of_heap[child]] = child;
			m_pos_in_seq[m_seq_of_heap[parent]] = parent;
			parent = child;
			child = 2 * parent + 1;
		}
	}

	void Q_AHC::update_Q(size_t row, size_t column, size_t id, bool flag, const std::vector<std::vector<bool>>& adjmatrix) {
		int size = adjmatrix.size();
		int interpolate_id;
		if (flag == true)
			interpolate_id = id / size;
		else
			interpolate_id = id % size;
		size_t cluster1 = 0, cluster2 = 0, cluster3 = 0;
		while (row != m_cluster[cluster1].cluster_id)
			cluster1++;
		while (column != m_cluster[cluster2].cluster_id)
			cluster2++;
		while (interpolate_id != m_cluster[cluster3].cluster_id)
			cluster3++;
		bool flag1 = false, flag2 = false;
		for (size_t i = 0; i < m_cluster[cluster3].member.size(); i++) {
			if (flag1 == false) {
				for (size_t j = 0; j < m_cluster[cluster1].member.size(); j++) {
					if (adjmatrix[m_cluster[cluster3].member[i]][m_cluster[cluster1].member[j]] == true) {
						flag1 = true;
						break;
					}
				}
			}
			if (flag2 == false) {
				for (size_t j = 0; j < m_cluster[cluster2].member.size(); j++) {
					if (adjmatrix[m_cluster[cluster3].member[i]][m_cluster[cluster2].member[j]] == true) {
						flag2 = true;
						break;
					}
				}
			}
		}
		if (flag1 == true && flag2 == true) {
			m_Q[id].second = m_Q[interpolate_id * size + row].second + m_Q[interpolate_id * size + column].second;
			interpolate_heap(id);
		}
		else if (flag1 == true && flag2 == false) {
			m_Q[id].second = m_Q[interpolate_id * size + row].second - 2 * m_a[column] * m_a[interpolate_id];
			interpolate_heap(id);
		}
		else if (flag1 == false && flag2 == true) {
			m_Q[id].second = m_Q[interpolate_id * size + column].second - 2 * m_a[row] * m_a[interpolate_id];
			interpolate_heap(id);
		}
	}

	bool Q_AHC::interpolate_heap(size_t id) {
		bool flag = false;
		int child = m_pos_in_seq[id];
		int parent = (child - 1) / 2;
		if (parent >= 0 && m_Q[id].second > m_Q[m_seq_of_heap[parent]].second) {
			while (child > 0) {
				if (m_Q[m_seq_of_heap[parent]].second >= m_Q[m_seq_of_heap[child]].second)
					break;
				auto temp = m_seq_of_heap[parent];
				m_seq_of_heap[parent] = m_seq_of_heap[child];
				m_seq_of_heap[child] = temp;
				m_pos_in_seq[m_seq_of_heap[child]] = child;
				m_pos_in_seq[m_seq_of_heap[parent]] = parent;
				child = parent;
				parent = (child - 1) / 2;
			}
			flag = true;
			return flag;
		}
		parent = m_pos_in_seq[id];
		child = 2 * parent + 1;
		if (child < m_seq_of_heap.size() && m_Q[id].second < m_Q[m_seq_of_heap[child]].second) {
			while (child < m_seq_of_heap.size()) {
				if (static_cast<size_t>(child) + 1 < m_seq_of_heap.size() && m_Q[m_seq_of_heap[child]].second < m_Q[m_seq_of_heap[child + 1]].second)
					child++;
				if (m_Q[m_seq_of_heap[parent]].second >= m_Q[m_seq_of_heap[child]].second)
					break;
				auto temp = m_seq_of_heap[parent];
				m_seq_of_heap[parent] = m_seq_of_heap[child];
				m_seq_of_heap[child] = temp;
				m_pos_in_seq[m_seq_of_heap[child]] = child;
				m_pos_in_seq[m_seq_of_heap[parent]] = parent;
				parent = child;
				child = 2 * parent + 1;
			}
			flag = true;
			return flag;
		}
		else {
			flag = true;
			return flag;
		}
	}

	void Q_AHC::update_heap(size_t max_index, const std::vector<std::vector<bool>>& adjmatrix) {
		int size = adjmatrix.size();
		size_t row = max_index / size, column = max_index % size;
		size_t k;
		erase_heap(max_index);
		for (size_t i = 0; i < size; i++) {
			k = i * size + column;
			if (m_pos_in_seq[k] != -1) 
				erase_heap(k);
			k = column * size + i;
			if (m_pos_in_seq[k] != -1) 
				erase_heap(k);
			k = i * size + row;
			if (m_pos_in_seq[k] != -1 && i != row)
				update_Q(row, column, k, true, adjmatrix);
			k = row * size + i;
			if (m_pos_in_seq[k] != -1 && row != i)
				update_Q(row, column, k, false, adjmatrix);
		}
	}

	void Q_AHC::clustering(int n, const std::vector<std::vector<bool>>& adjmatrix) {
		build_heap();
		while (m_num_cluster > n) {
			auto max = m_Q[m_seq_of_heap.front()];
			if (max.second > 0) {
				size_t cluster1 = 0, cluster2 = 0;
				while (max.first.first != m_cluster[cluster1].cluster_id)
					cluster1++;
				while (max.first.second != m_cluster[cluster2].cluster_id)
					cluster2++;
				update_heap(m_seq_of_heap[0], adjmatrix);
				int n = 0;
				for (size_t i = 0; i < m_cluster[cluster1].member.size(); i++) {
					for (size_t j = 0; j < m_cluster[cluster2].member.size(); j++) {
						if (adjmatrix[m_cluster[cluster2].member[j]][m_cluster[cluster1].member[i]] == true)
							n++;
					}
				}
				if (m_a[m_cluster[cluster1].cluster_id] != -1 && m_a[m_cluster[cluster2].cluster_id] != -1) {
					m_a[m_cluster[cluster1].cluster_id] = ((m_a[m_cluster[cluster1].cluster_id] + m_a[m_cluster[cluster2].cluster_id]) * (2 * adjmatrix.size()) - n) / (2 * adjmatrix.size());
					m_a[m_cluster[cluster2].cluster_id] = -1;
				}
				else
					throw "Error,cluster id cannot be -1";
				for (size_t i = 0; i < m_cluster[cluster2].member.size(); i++) {
					m_cluster[cluster1].member.push_back(m_cluster[cluster2].member[i]);
				}
				m_cluster.erase(m_cluster.begin() + cluster2);
				m_num_cluster--;
			}
			
			else {
				std::cout << "ERROR!" << std::endl;
				throw "Error,cluster failed!";
			}
		}
	}
}