#include "link_sort.h"

namespace NS {
	void link_sort::ObjectiveSortedGraphMergeChildrenSort(const std::vector<std::vector<double>>& data, std::vector<int>& dataRank) {
		int nodeNum(data.size());
		if (nodeNum == 0)return;
		dataRank.resize(data.size());
		int objNum = data.front().size();
		std::vector<std::vector<int>> nodeObjectiveHead(nodeNum);
		for (auto & it : nodeObjectiveHead) {
			it.resize(objNum);
		}
		std::vector<std::vector<L_Node>> nodeObjectiveIndex(nodeNum);
		for (int i(0); i < nodeNum; i++) {
			nodeObjectiveIndex[i].resize(objNum);
			for (auto & it : nodeObjectiveIndex[i]) {
				it.m_id = i;
			}
		}
		L_Node LNend;
		std::vector<std::vector<L_Node>> totalObejctiveLink(objNum);
		std::vector<int> nodeSum(nodeNum, 0);
		std::vector<int> nodeFromObjective(nodeNum, 0);

		// initialize the objectiveGraph   time: O(m*n*lg(n)))

		{
			std::vector<int> nodeObjBeforeCur(nodeNum, nodeNum + 1);
			// temp memory, first: data,second : index
			std::vector<std::pair<double, int>> curObjLink(nodeNum);
			std::vector<int> preSum(nodeNum);
			//std::vector<int> objNodeIndex(m_nodeNum, 0);// remember  the index of nodex at current objective
			int objSortedId(0), objIndex(0), nodeIndex(0);
			double beforeObj(0);
			for (objIndex = 0; objIndex <objNum; ++objIndex) {
				for (nodeIndex = 0; nodeIndex <nodeNum; ++nodeIndex) {
					curObjLink[nodeIndex].first = data[nodeIndex][objIndex];
					curObjLink[nodeIndex].second = nodeIndex;
				}
				sort(curObjLink.begin(), curObjLink.end());
				objSortedId = -1;
				beforeObj = -1e9;
				for (const auto& it : curObjLink) {
					if (it.first > beforeObj) {
						preSum[++objSortedId] = 0;
						beforeObj = it.first;
					}
					nodeObjectiveHead[it.second][objIndex] = objSortedId;
					nodeSum[it.second] += objSortedId;
					++preSum[objSortedId];
				}
				totalObejctiveLink[objIndex].resize(++objSortedId);
				for (auto & it : totalObejctiveLink[objIndex]) {
					it.m_after = &LNend;
				}
				// to get the smallest Index of objective of each node
				for (nodeIndex = objSortedId - 2; nodeIndex >= 0; --nodeIndex) {
					preSum[nodeIndex] += preSum[nodeIndex + 1];
				}

				for (nodeIndex = 0; nodeIndex < nodeNum; ++nodeIndex) {
					if (nodeObjBeforeCur[nodeIndex] > preSum[nodeObjectiveHead[nodeIndex][objIndex]]) {
						nodeObjBeforeCur[nodeIndex] = preSum[nodeObjectiveHead[nodeIndex][objIndex]];
						nodeFromObjective[nodeIndex] = objIndex;
					}
				}

			}
		}
		std::vector<L_Node> nodeCurrentIndex(nodeNum);
		for (int i(0); i < nodeNum; ++i) {
			nodeCurrentIndex[i].m_id = i;
		}
		L_Node visitingNodeHead(-1, false, nullptr, &LNend);
		std::vector<L_Node> nodeChildren(nodeNum);
		for (auto & it : nodeChildren) {
			it.m_after = &LNend;
		}
		std::vector<std::pair<int, int>> nodeObjectiveSum(nodeNum);
		int curNodeNum(nodeNum);
		for (int i(0); i < curNodeNum; ++i) {
			nodeObjectiveSum[i].first = nodeSum[i];
			nodeObjectiveSum[i].second = i;
		}
		int curRank(0);
		L_Node * curPointer(nullptr);
		L_Node * childrenPointer(nullptr);
		int firstIndex(nodeNum), lastIndex(nodeNum - 1);
		int objIndex(0);
		int curId(0), lastId(0);
		L_Node * NodeBefore(nullptr);
		while (curNodeNum) {

			sort(nodeObjectiveSum.begin(), nodeObjectiveSum.begin() + curNodeNum);
			firstIndex = curNodeNum;
			lastIndex = curNodeNum - 1;
			while (--firstIndex >= 0) {
				// two pointer iterator
				while (nodeObjectiveSum[firstIndex].first<nodeObjectiveSum[lastIndex].first) {
					lastId = nodeObjectiveSum[lastIndex].second;
					// insert the node into the objective network
					for (objIndex = 0; objIndex < objNum; ++objIndex) {
						insertL_Node(&totalObejctiveLink[objIndex][nodeObjectiveHead[lastId][objIndex]], &nodeObjectiveIndex[lastId][objIndex]);
					}
					--lastIndex;
				}
				curId = nodeObjectiveSum[firstIndex].second;
				insertL_Node(&visitingNodeHead, &nodeCurrentIndex[curId]);
				int SOLsize(totalObejctiveLink[nodeFromObjective[curId]].size());
				for (int index = nodeFromObjective[curId]; index< SOLsize; ++index) {
					NodeBefore = &totalObejctiveLink[nodeFromObjective[curId]][index];
					while (NodeBefore->m_after->m_isUsed) {
						NodeBefore = NodeBefore->m_after;
						if (compareDominated(data[curId], data[NodeBefore->m_id], objNum)) {
							for (auto & it : nodeObjectiveIndex[NodeBefore->m_id]) {
								deleteL_Node(&it);
							}
							deleteL_Node(&nodeCurrentIndex[NodeBefore->m_id]);
							insertL_Node(&nodeChildren[curId], &nodeCurrentIndex[NodeBefore->m_id]);
						}
					}
				}
				//	mergeChildren(totalObejctiveLink[nodeFromObjective[curId]], nodeObjectiveHead[curId][nodeFromObjective[curId]], curId, &nodeChildren[curId], nodeCurrentIndex, nodeObjectiveIndex, data, objNum);

			}
			curPointer = &visitingNodeHead;
			curNodeNum = 0;
			while (curPointer->m_after->m_isUsed) {
				curPointer = curPointer->m_after;
				dataRank[curPointer->m_id] = curRank;

				// move its children to no-dominated link
				childrenPointer = &nodeChildren[curPointer->m_id];
				while (childrenPointer->m_after->m_isUsed) {
					childrenPointer = childrenPointer->m_after;
					nodeObjectiveSum[curNodeNum].first = nodeSum[childrenPointer->m_id];
					nodeObjectiveSum[curNodeNum++].second = childrenPointer->m_id;
				}
				// delete the front node from objective graph
				if (nodeObjectiveIndex[curPointer->m_id][0].m_isUsed) {
					for (auto & it : nodeObjectiveIndex[curPointer->m_id]) {
						deleteL_Node(&it);
					}
				}
			}
			visitingNodeHead.m_after = &LNend;
			++curRank;
		}


	}
	void link_sort::RankObjectiveLinkSort(const std::vector<std::vector<double>>& data, std::vector<int>& dataRank) {
		int nodeNum(data.size());
		if (nodeNum == 0)return;
		dataRank.assign(nodeNum, -1);
		int objNum(data.front().size());
		std::vector<int> objectiveLastRank(objNum, 0);
		L_Node LNend;
		std::vector<std::vector<L_Node>> rankObjeciveHead(objNum);
		for (auto & it : rankObjeciveHead) {
			it.resize(nodeNum);
			for (auto & it2 : it) {
				it2.m_after = &LNend;
			}
		}
		std::vector<std::vector<L_Node>> nodeObjectiveNode(objNum);


		for (auto & it : nodeObjectiveNode) {
			it.resize(nodeNum);
			for (int nodeIndex(0); nodeIndex < nodeNum; ++nodeIndex) {
				it[nodeIndex].m_id = nodeIndex;
			}
		}
		std::vector<std::vector<std::pair<double, int>>> objectiveSortedNode(objNum);
		std::vector<std::vector<int>> objectiveSortedSameNode(objNum);// remember the index of node with the same objective value
		std::vector<int> nodeObjectiveRankSum(nodeNum, 0);
		int curRank(0);
		double beforeObjective(-1);
		int sameNode(0);
		for (int objIndex(0); objIndex < objNum; ++objIndex) {
			objectiveSortedNode[objIndex].resize(nodeNum);
			objectiveSortedSameNode[objIndex].resize(nodeNum);
			for (int nodeIndex(0); nodeIndex < nodeNum; ++nodeIndex) {
				objectiveSortedNode[objIndex][nodeIndex].first = data[nodeIndex][objIndex];
				objectiveSortedNode[objIndex][nodeIndex].second = nodeIndex;
			}
			sort(objectiveSortedNode[objIndex].begin(), objectiveSortedNode[objIndex].end());
			curRank = -1;
			sameNode = -1;
			beforeObjective = -1;
			for (int nodeIndex(0); nodeIndex < nodeNum; ++nodeIndex) {
				if (beforeObjective<objectiveSortedNode[objIndex][nodeIndex].first) {
					beforeObjective = objectiveSortedNode[objIndex][nodeIndex].first;
					++curRank;
					sameNode = 0;
				}
				else ++sameNode;
				nodeObjectiveRankSum[objectiveSortedNode[objIndex][nodeIndex].second] += curRank;
				objectiveSortedSameNode[objIndex][nodeIndex - sameNode] = nodeIndex;
			}
		}

		int totalSolvedNode(0);
		int nodeIndex(0);
		int objIndex(0);
		std::vector<int> parents(nodeNum, nodeNum);
		std::vector<int> objCompareIndex(nodeNum + 1, 0);
		int iter(0), tmp(0);
		L_Node * iterPointer(nullptr);
		int ret = 0;
		while (totalSolvedNode < nodeNum) {
			for (objIndex = 0; objIndex < objNum; ++objIndex) {
				// sort the same objective value node based on the sum of rank index
				if (objectiveSortedSameNode[objIndex][nodeIndex] != nodeIndex) {
					tmp = objectiveSortedSameNode[objIndex][nodeIndex];
					for (iter = nodeIndex; iter <= tmp; ++iter) {
						objectiveSortedNode[objIndex][iter].first = nodeObjectiveRankSum[objectiveSortedNode[objIndex][iter].second];
					}
					sort(objectiveSortedNode[objIndex].begin() + nodeIndex, objectiveSortedNode[objIndex].begin() + tmp + 1);
					for (iter = nodeIndex; iter <= tmp; ++iter) {
						objectiveSortedSameNode[objIndex][iter] != iter;
					}
				}
				tmp = objectiveSortedNode[objIndex][nodeIndex].second;
				if (tmp == 536) {
					int stop = -1;
				}
				if (dataRank[tmp] == -1) {
					iter = objectiveLastRank[objIndex];
					while (iter >= 0 && dataRank[tmp] == -1) {
						iterPointer = &rankObjeciveHead[objIndex][iter];
						while (iterPointer->m_after->m_isUsed) {
							iterPointer = iterPointer->m_after;
							if (objCompareIndex[iterPointer->m_id] >= objNum*totalSolvedNode) {
								objCompareIndex[iterPointer->m_id] %= objNum;
							}
							else objCompareIndex[iterPointer->m_id] = 0;
							ret = compareDominated(data[iterPointer->m_id], data[tmp], objCompareIndex[iterPointer->m_id], objNum);
							if (ret == 1) {
								dataRank[tmp] = ++iter;
								break;
							}
							else if (ret == 0) {
								dataRank[tmp] = iter;
								break;
							}
							objCompareIndex[parents[iterPointer->m_id]] = std::max(objCompareIndex[parents[iterPointer->m_id]], objNum*totalSolvedNode + objCompareIndex[iterPointer->m_id]);
						}
						--iter;
					}
					if (dataRank[tmp] == -1)dataRank[tmp] = 0;
					if (++totalSolvedNode == nodeNum)break;

				}
				objectiveLastRank[objIndex] = std::max(objectiveLastRank[objIndex], dataRank[tmp]);
				insertL_Node(&rankObjeciveHead[objIndex][dataRank[tmp]], &nodeObjectiveNode[objIndex][tmp]);
			}
			++nodeIndex;
		}
	}
	bool link_sort::compareDominated(const std::vector<double>& father, const std::vector<double>& son, int objSize) {
		for (int objIndex(0); objIndex < objSize; ++objIndex) {
			if (father[objIndex] > son[objIndex]) {
				return false;
			}
		}
		return true;
	}
	int link_sort::compareDominated(const std::vector<double>& father, const std::vector<double>& son, int & fromIndex, int objSize) {
		int ret = 0;
		for (; fromIndex < objSize; ++fromIndex) {
			if (father[fromIndex] > son[fromIndex]) {
				return -1;
			}
			else if (father[fromIndex] < son[fromIndex]) {
				ret = 1;
			}
		}
		return ret;
	}
}