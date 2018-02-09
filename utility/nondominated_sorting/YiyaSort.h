#ifndef YIYASORT_H
#define YIYASORT_H

#include<vector>
#include<algorithm>
//using namespace std;
namespace NDS {
	class YiyaSort {
		struct L_Node {
			int m_id;
			L_Node * m_before;
			L_Node * m_after;
			bool m_isUsed;
			L_Node(int id = -1, bool isUsed = false, L_Node* before = nullptr, L_Node * after = nullptr) :m_id(id),
				m_before(before), m_after(after), m_isUsed(isUsed) {};
		};
	public:
		void ObjectiveSortedGraphMergeChildrenSort(const std::vector<std::vector<double>> & data, std::vector<int> & dataRank);
		// idea  binary search of rank??
		// recommemded
		void RankObjectiveLinkSort(const std::vector<std::vector<double>> & data, std::vector<int> & dataRank);
	private:
		bool compareDominated(const std::vector<double> & father, const std::vector<double> & son, int objSize);
		int compareDominated(const std::vector<double> & father, const std::vector<double> & son, int& fromIndex, int objSize);
		// basic function
		// L_Node
		void insertL_Node(L_Node * head, L_Node * cur) {
			cur->m_after = head->m_after;
			cur->m_before = head;
			cur->m_after->m_before = head->m_after = cur;
			cur->m_isUsed = true;
		}
		void deleteL_Node(L_Node * cur) {
			cur->m_after->m_before = cur->m_before;
			cur->m_before->m_after = cur->m_after;
			cur->m_isUsed = false;
		}
	};
}

#endif // !YIYASORT_H