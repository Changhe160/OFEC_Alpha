#pragma once

#include"../random/newran.h"
#include<list>
#include<vector>
#include<queue>
#include<stack>
#include<iostream>
#include<algorithm>

using namespace std;
/*

Question(1): ID or pointer ?

*/


namespace NDS {


	extern struct DG_Node;
	struct DGlinkNode {
		int m_toId;
		DGlinkNode * m_before;
		DGlinkNode * m_after;
		bool m_isUsed;
		DGlinkNode(int toId=0, bool isUsed=false, DGlinkNode* before=nullptr, DGlinkNode * after=nullptr) :m_toId(toId),
			m_before(before),m_after(after), m_isUsed(isUsed){};
	};


	struct DG_Node {
		int m_id;
		int m_rank;
		//bool m_isChange;
		vector<double> m_obj;
		DGlinkNode m_directParens;
		DGlinkNode m_parents;
		DGlinkNode m_directChildren;
		DGlinkNode m_children;
		DGlinkNode m_end;
		DGlinkNode m_indexPosition;


		DG_Node() :m_id(0), m_rank(0){

			m_directParens.m_after = m_parents.m_after = m_directChildren.m_after= m_children.m_after = &m_end;
			m_end.m_isUsed = false;
		};
	};


	class ObjecitveLink_Graph_Sort {

	private:
		int m_nodeNum;
		int m_objNum;
		unsigned m_maxSize;
		bool m_isChange;
		const int m_firstRank = 0;
		vector<DG_Node> m_Dgraph;
		stack<int> m_NotUsedId;// id management
		DGlinkNode m_indexHead;
		DGlinkNode m_indexEnd;
		vector<vector<DGlinkNode>> m_directDominated;
		vector<vector<DGlinkNode>> m_dominated;
		
	public:

		/*
		
		*/

		//unsigned m_maxSize;// the maximum size of nodes to be deal with
		//the maximun size of nodes to be dealt with
		ObjecitveLink_Graph_Sort(unsigned maxSize) :m_isChange(false), m_maxSize(maxSize), m_Dgraph(maxSize), m_nodeNum(0), 
			m_objNum(0), m_directDominated(maxSize), m_dominated(maxSize){
			int tmpid(maxSize);
			while (tmpid>=0) {
				m_NotUsedId.push(--tmpid);
			}
			initObject();
		}
		//recommemded: the maximun size of nodes to be dealt with, 
		ObjecitveLink_Graph_Sort(unsigned maxSize, const vector<vector<double>>  &data) :m_isChange(false), m_maxSize(maxSize), m_Dgraph(maxSize),m_nodeNum(data.size()), m_objNum(0), m_directDominated(maxSize), m_dominated(maxSize) {
			if (data.empty())return;
			m_objNum = data.front().size();
			initObject();
			int useid(0);
			for (; useid < m_nodeNum; ++useid) {
				setDgraphNode(useid, data[useid]);
			}
			int tmpid(maxSize);
			while (tmpid > useid) {
				m_NotUsedId.push(--tmpid);
			}
		}

		void initGraphLinkSort() {

			if (m_nodeNum == 0 || m_objNum == 0)return;
			vector<vector<list<int>>> totalObjLink(m_nodeNum);//set the objLink based on the sorted Index
			vector<vector<int>> nodeObjIndex(m_nodeNum);// remember each node's sorted Index at each objective
			for (auto & it : nodeObjIndex) {
				it.resize(m_objNum);
			}
			vector<pair<int, int>> nodeObjIndexSum(m_nodeNum);// first is ObjIndexSum ,second is Index
			for (int i(0); i < m_nodeNum; ++i) {
				nodeObjIndexSum[i].first = 0;
				nodeObjIndexSum[i].second = i;
			}

			vector<int> nodeMinObj(m_nodeNum, 0);// remember at which objectives the number of node before itself  is smallest
												 //vector<int> nodeMinObjIndex(m_nodeNum, 0);

												 // initilize 
			{
				vector<int> nodeObjBeforeCur(m_nodeNum, m_nodeNum + 1);


				// temp memory, first: data,second : index
				vector<pair<double, int>> curObjLink(m_nodeNum);
				vector<int> preSum(m_nodeNum);
				//vector<int> objNodeIndex(m_nodeNum, 0);// remember  the index of nodex at current objective

				int objSortedId(0), objIndex(0);
				double beforeObj(0);
				for (int i(0); i < m_objNum; ++i) {
					objIndex = i;
					for (int j(0); j < m_nodeNum; ++j) {
						curObjLink[j].first = m_Dgraph[j].m_obj[objIndex];
						curObjLink[j].second = j;
					}
					sort(curObjLink.begin(), curObjLink.end());
					objSortedId = -1;
					beforeObj = -1e9;
					for (const auto& it : curObjLink) {
						if (it.first > beforeObj) {
							preSum[++objSortedId] = 0;
							beforeObj = it.first;
						}

						nodeObjIndex[it.second][objIndex] = objSortedId;
						nodeObjIndexSum[it.second].first += objSortedId;
						++preSum[objSortedId];
					}
					totalObjLink[objIndex].resize(++objSortedId);

					// to get the smallest Index of objective of each node
					for (int j(objSortedId - 2); j >= 0; --j) {
						preSum[j] += preSum[j + 1];
					}

					for (int j(0); j < m_nodeNum; ++j) {
						if (nodeObjBeforeCur[j] > preSum[nodeObjIndex[j][objIndex]]) {
							nodeObjBeforeCur[j] = preSum[nodeObjIndex[j][objIndex]];
							nodeMinObj[j] = objIndex;
						}
					}

				}
			}


			// to solve the node based on its sum 
			sort(nodeObjIndexSum.begin(), nodeObjIndexSum.end());

			int firstIndex(m_nodeNum), lastIndex(m_nodeNum - 1);

			vector<int> visited(m_maxSize, -1);
			int curNode(0);
			int minObjIndex(0);
			int objFrom(0);
			int objCompareId(0);
			while (--firstIndex >= 0) {
				while (nodeObjIndexSum[firstIndex].first<nodeObjIndexSum[lastIndex].first) {
					// insert the node into the objective network
					for (int i(0); i < m_objNum; ++i) {
						totalObjLink[i][nodeObjIndex[nodeObjIndexSum[lastIndex].second][i]].emplace_front(nodeObjIndexSum[lastIndex].second);
					}
					--lastIndex;
				}
				curNode = nodeObjIndexSum[firstIndex].second;
				minObjIndex = nodeMinObj[curNode];
				objFrom = nodeObjIndex[curNode][minObjIndex];
				for (; objFrom < totalObjLink[minObjIndex].size(); ++objFrom) {
					for (auto it : totalObjLink[minObjIndex][objFrom]) {
						if (visited[it] != curNode) {
							if (compareDominate(curNode, it, objCompareId) == 1) {
								calDominateLatter(curNode, it, visited);
							}
							else {
								calNotDominateLatter(curNode, it, objCompareId, visited);
							}
						}
					}
				}

			}

			vector<int> sortedId(m_nodeNum);
			for (int i = 0; i < m_nodeNum; ++i) {
				sortedId[i] = nodeObjIndexSum[i].second;
			}
			topoSort(sortedId);
		}
		void initObjectSort(OFEC::random& rand) {
			int objIndex = rand.next()*m_objNum;
			vector<pair<double, int>> v_objId(m_nodeNum);
			for (int i(0); i < m_nodeNum; ++i) {
				v_objId[i].first = m_Dgraph[i].m_obj[objIndex];
				v_objId[i].second = i;
			}
			sort(v_objId.begin(), v_objId.end());
			vector<int> visited(m_maxSize);
			int ret(0), objId(0);
			int curId(0), compareId(0);
			for (int i(m_nodeNum - 1); i >= 0; --i) {
				curId = v_objId[i].second;
				for (int j = i + 1; j < m_nodeNum; j++) {
					compareId= v_objId[j].second;
					if (visited[compareId] != curId) {
						ret = compareDominate(curId, compareId, objId);
						if (ret == -1) {
							calNotDominateLatter(curId, compareId, objId, visited);
						}
						else if (ret == 0) {
							calSameLatter(curId, compareId);
							break;
						}
						else {
							calDominateLatter(curId, compareId, visited);
						}
					}
				}
			}

			vector<int> sortedId(m_nodeNum);
			for (int i = 0; i < m_nodeNum; ++i) {
				sortedId[i] = v_objId[i].second;
			}
			topoSort(sortedId);
		}


		void testIndexLink() {
			DGlinkNode * iter(&m_indexHead);
			int totalNode(0);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				totalNode++;
				cout << iter->m_toId << endl;
			}
			cout << "totalNode:\t" << totalNode << endl;
			cout << endl;
		}
		void insert(const vector<vector<double>> & data,OFEC::random & rand) {
			m_isChange = true;
			m_nodeNum += data.size();
			vector<pair<double, int>> tmpObjNode(m_nodeNum);
			vector<bool> isNew(m_maxSize, false);
			int objIndex(rand.next()* m_objNum);
			int i(0);
			DGlinkNode * iter(&m_indexHead);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				tmpObjNode[i].second = iter->m_toId;
				tmpObjNode[i++].first = m_Dgraph[iter->m_toId].m_obj[objIndex];
			}
			for (const auto & it:data) {
				isNew[tmpObjNode[i].second = setDgraphNode(it)] = true;
				tmpObjNode[i++].first = it[objIndex];


				//if (tmpObjNode[i].second == 73) {
				//	int stop = -1;
				//}
				//system("cls");
				//testIndexLink();

				//cout << endl;
			}

			//testIndexLink();



			sort(tmpObjNode.begin(), tmpObjNode.end());
			vector<int> visited(m_maxSize, -1);
			int ret(0), objId(0);
			int curId(0), compareId(0);
			for (int i(m_nodeNum - 1); i >= 0; i--) {
				curId = tmpObjNode[i].second;
				if (!isNew[curId]) {
					sortedOldNode(curId, visited);
				}
				else {
					for (int j = i + 1; j < m_nodeNum; ++j) {
						compareId = tmpObjNode[j].second;
						ret = compareDominate(curId, compareId, objId);
						if (ret == -1) {
							calNotDominateLatter(curId, compareId, objId, visited);
						}
						else if (ret == 0) {
							calSameLatter(curId, compareId);
							break;
						}
						else {
							calDominateLatter(curId, compareId, visited);
						}
					}

					for (int j = i - 1; j >= 0; --j) {
						compareId = tmpObjNode[j].second;
						if (!isNew[compareId]) {
							if (visited[compareId] != curId) {
								ret = compareDominate(compareId, curId, objId);

								if (ret == -1) {
									calNotDominateBefore(curId, compareId, objId, visited);
								}
								else if (ret == 1) {
									calDominateBefore(curId, compareId, visited);
								}
							}
						}
					}
				}
			}
			vector<int> sortedId(m_nodeNum);
			for (int i(0); i < m_nodeNum; ++i) {
				sortedId[i] = tmpObjNode[i].second;
			}
			topoSort(sortedId);
		}


		// deleteID index  time O(m_nodexNum) + rearrage DG_graph
		void deleteNode(vector<int>  delId) {
			sort(delId.begin(), delId.end(), isgreater<int, int>);
			int curId(m_nodeNum);
			auto itDelId(delId.begin());
			DGlinkNode* itIndex(&m_indexHead);
		//	DGlinkNode* itParents(nullptr);
			m_isChange = true;
			while (itIndex->m_after->m_isUsed) {
				itIndex = itIndex->m_after;
				if (--curId == *itDelId) {
					
				//	itParents =&m_Dgraph[itIndex->m_toId].m_directParens;
					

					deleteNode(itIndex->m_toId);
					deleteLinkNode(itIndex);
					++itDelId;
					if (itDelId == delId.end())break;
				}

			}
			m_nodeNum -= delId.size();
		}

		void getRank(std::vector<int>& nodeRank) {

			if (m_isChange) {
				cout << "not handled??" << endl;
			}
			nodeRank.resize(m_nodeNum);
			int id(m_nodeNum);
			DGlinkNode * iter(&m_indexHead);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				nodeRank[--id] = m_Dgraph[iter->m_toId].m_rank;
			}
		}


		void sortAfterChange(OFEC::random & rand) {
			vector<pair<double, int>> tmpObjNode(m_nodeNum);
			int objIndex(rand.next()* m_objNum);
			DGlinkNode * iter(&m_indexHead);
			int i(0);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				tmpObjNode[i].second = iter->m_toId;
				tmpObjNode[i++].first = m_Dgraph[iter->m_toId].m_obj[objIndex];
			}
			sort(tmpObjNode.begin(), tmpObjNode.end());
			vector<int> visited(m_maxSize, -1);
			for (int i(m_nodeNum - 1); i >= 0; i--) {
				sortedOldNode(tmpObjNode[i].second, visited);
				
			}
			vector<int> sortedId(m_nodeNum);
			for (int i(0); i < m_nodeNum; ++i) {
				sortedId[i] = tmpObjNode[i].second;
			}
			topoSort(sortedId);
		}
	private:
		
		void sortedOldNode(int curId,vector<int> & visited) {
			queue<int> queDirectSons;
			DGlinkNode * iter(&m_Dgraph[curId].m_children);
			DGlinkNode * iter2(nullptr);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				if (visited[iter->m_toId] != curId) {
					queDirectSons.push(iter->m_toId);
				}
				iter2 = &m_Dgraph[iter->m_toId].m_directChildren;
				while (iter2->m_after->m_isUsed) {
					iter2 = iter2->m_after;
					visited[iter2->m_toId] = curId;
				}
			}
			iter = &m_Dgraph[curId].m_directChildren;
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				if (visited[iter->m_toId] == curId) {
					deleteLinkNode(iter);
					deleteLinkNode(&m_directDominated[iter->m_toId][curId]);
				}
				else {
					visited[iter->m_toId] = curId;
				}
			}

			while (!queDirectSons.empty()) {
				if (visited[queDirectSons.front()] != curId) {
					visited[queDirectSons.front()] = curId;
					addDirectDominate(curId, queDirectSons.front());
				}
				queDirectSons.pop();
			}
			
		}
		void initObject() {

			for (int i(0); i < m_maxSize; ++i) {
				m_Dgraph[i].m_id = i;
				m_Dgraph[i].m_indexPosition.m_toId = i;
				m_Dgraph[i].m_indexPosition.m_isUsed = true;
			}

			for (int i(0); i < m_maxSize; ++i) {
				m_directDominated[i].resize(m_maxSize);
				for (int j(0); j < m_maxSize; ++j) {
					m_directDominated[i][j].m_toId = j;
					m_directDominated[i][j].m_isUsed = false;
				}
			}

			for (int i(0); i < m_maxSize; ++i) {
				m_dominated[i].resize(m_maxSize);
				for (int j(0); j < m_maxSize; ++j) {
					m_dominated[i][j].m_toId = j;
					m_dominated[i][j].m_isUsed = false;
				}
			}

			m_indexHead.m_after = &m_indexEnd;
			m_indexEnd.m_before = &m_indexHead;
			m_indexEnd.m_isUsed = false;
		}
		// time O(n+m)
		void topoSort(const vector<int>& sortedId) {
			DGlinkNode * iter(&m_indexHead);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				m_Dgraph[iter->m_toId].m_rank = m_firstRank;
			}


		//	cout << "new" << endl;
			int tmpRank(0);
			for (auto it : sortedId) {
				iter = &m_Dgraph[it].m_directChildren;
				tmpRank = m_Dgraph[it].m_rank+1;

		//		cout << "father:: " << it << "\t";
				while (iter->m_after->m_isUsed) {
					iter = iter->m_after;
			//		cout << iter->m_toId << "\t";
					m_Dgraph[iter->m_toId].m_rank = max(m_Dgraph[iter->m_toId].m_rank, tmpRank);
				}

			//	cout << endl;
			}
			m_isChange = false;
		}
		void addDirectDominate(int father, int son) {
			insertLinkNode(&m_Dgraph[father].m_directChildren, &m_directDominated[father][son]);
			insertLinkNode(&m_Dgraph[son].m_directParens, &m_directDominated[son][father]);
		}
		void addDominate(int father, int son) {
			insertLinkNode(&m_Dgraph[father].m_children, &m_dominated[father][son]);
			insertLinkNode(&m_Dgraph[son].m_parents, &m_dominated[son][father]);
		}
		void insertLinkNode(DGlinkNode * head, DGlinkNode * cur) {
			cur->m_after = head->m_after;
			cur->m_before = head;
			cur->m_after->m_before = head->m_after = cur;
			cur->m_isUsed = true;
		}
		void deleteLinkNode(DGlinkNode * cur) {
			cur->m_after->m_before = cur->m_before;
			cur->m_before->m_after = cur->m_after;
			cur->m_isUsed = false;
		}

		void deleteLink(int id, DGlinkNode * head, vector<vector<DGlinkNode>> & relationship) {
			while (head->m_after->m_isUsed) {
				head = head->m_after;
				//m_Dgraph[head->m_toId].m_isChange = true;
				deleteLinkNode(&relationship[head->m_toId][id]);
				deleteLinkNode(head);
			}
		}
		void deleteNode(int delId) {

			m_NotUsedId.push(delId);
			deleteLink(delId,&m_Dgraph[delId].m_directParens, m_directDominated);
			deleteLink(delId, &m_Dgraph[delId].m_directChildren, m_directDominated);
			deleteLink(delId, &m_Dgraph[delId].m_children, m_dominated);
			deleteLink(delId, &m_Dgraph[delId].m_parents, m_dominated);
		}

		void setDgraphNode(int index, const std::vector<double>& data) {

			insertLinkNode(&m_indexHead, &m_Dgraph[index].m_indexPosition);
			m_Dgraph[index].m_obj = data;
		}
		int setDgraphNode(const std::vector<double>& data) {
			int index(m_NotUsedId.top());
			m_NotUsedId.pop();
			setDgraphNode(index, data);
			return index;
		}
		void calSameLatter(int curNode, int identical) {
			DGlinkNode * iter(&m_Dgraph[identical].m_directChildren);
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				addDirectDominate(curNode, iter->m_toId);
			}
			iter = &m_Dgraph[identical].m_children;
			while (iter->m_after->m_isUsed) {
				iter = iter->m_after;
				addDominate(curNode, iter->m_toId);
			}
		}
		void calDominateLatter(int curNode, int children, vector<int>& visited) {
			if (visited[children] == curNode)return;
			addDirectDominate(curNode, children);
			addDominate(curNode, children);
			visited[children] = curNode;
			queue<int> sons;
			sons.push(children);
			DGlinkNode * iter(nullptr);
			while (!sons.empty()) {
				iter = &m_Dgraph[sons.front()].m_directChildren;
				sons.pop();
				while (iter->m_after->m_isUsed) {
					iter = iter->m_after;
					if (visited[iter->m_toId] != curNode) {
						visited[iter->m_toId] = curNode;
						addDominate(curNode, iter->m_toId);
						sons.push(iter->m_toId);
					}
				}

			}
		}

		void calDominateBefore(int curNode, int parent, vector<int>& visited) {
			if (visited[parent] == curNode)return;
			addDominate(parent, curNode);
			visited[parent] = curNode;
			queue<int> father;
			father.push(parent);
			DGlinkNode * iter(nullptr);
			while (!father.empty()) {
				iter = &m_Dgraph[father.front()].m_directParens;
				father.pop();
				while (iter->m_after->m_isUsed) {
					iter = iter->m_after;
					if (visited[iter->m_toId] != curNode) {
						visited[iter->m_toId] = curNode;
						addDominate(iter->m_toId,curNode);
						father.push(iter->m_toId);
					}
				}

			}
		}

		void calNotDominateBefore(int curNode, int NDpeers, int objId, vector<int>& visited) {
			if (visited[NDpeers] == curNode)return;
			visited[NDpeers] = curNode;
			queue<int> father;
			father.push(NDpeers);
			DGlinkNode * iter(nullptr);
			while (!father.empty()) {
				iter = &m_Dgraph[father.front()].m_directParens;
				father.pop();

				while (iter->m_after->m_isUsed) {
					iter = iter->m_after;
					if (visited[iter->m_toId] != curNode&&m_Dgraph[curNode].m_obj[objId]<m_Dgraph[iter->m_toId].m_obj[objId]) {
						visited[iter->m_toId] = curNode;
						father.push(iter->m_toId);
					}
				}
			}
		}
		void calNotDominateLatter(int curNode, int NDpeers, int objId, vector<int>& visited) {
			if (visited[NDpeers] == curNode)return;
			visited[NDpeers] = curNode;
			queue<int> sons;
			sons.push(NDpeers);
			DGlinkNode * iter(nullptr);
			while (!sons.empty()) {
				iter=&m_Dgraph[sons.front()].m_directChildren;
				sons.pop();

				while (iter->m_after->m_isUsed) {
					iter = iter->m_after;
					if (visited[iter->m_toId] != curNode&&m_Dgraph[curNode].m_obj[objId]>m_Dgraph[iter->m_toId].m_obj[objId]) {
						visited[iter->m_toId] = curNode;
						sons.push(iter->m_toId);
					}
				}
			}
		}
		int compareDominate(int curNode,int  NDpeers, int & objId) {
			if (curNode == 1 && NDpeers == 3) {
				int stop = -1;
			}
			bool equal(true);
			for (int i(0); i < m_objNum; ++i) {
				if (m_Dgraph[curNode].m_obj[i] > m_Dgraph[NDpeers].m_obj[i]) {
					objId = i;
					return -1;
				}
				else if (equal&&m_Dgraph[curNode].m_obj[i] < m_Dgraph[NDpeers].m_obj[i]) {
					objId = i;
					equal = false;
				}
			}
			if (equal) {
				return 0;
			}
			else return 1;
		}
	};



}