#ifndef ENS_NDT_NDNODE_H
#define ENS_NDT_NDNODE_H

#include "NDSplit.h"

namespace ENS_NDT {
	class NDNode {
	private:
		int BucketSize;
		std::shared_ptr<NDSplit> Split;
		std::vector<std::vector<double>*>* Points;
		std::unique_ptr<NDNode> worseNode;
		std::unique_ptr<NDNode> betterNode;

	public:
		int get_BucketSize() { return BucketSize; }
		std::shared_ptr<NDSplit>& get_Split() { return Split; }
		std::unique_ptr<NDNode>& get_worseNode() { return worseNode; }
		std::unique_ptr<NDNode>& get_betterNode() { return betterNode; }
		bool IsBranch() const { return Points == nullptr; }
		bool IsLeaf() const { return Points != nullptr; }

	public:
		NDNode(std::shared_ptr<NDSplit>& split, int bucketSize) : BucketSize(bucketSize), Split(split), Points(new std::vector<std::vector<double>*>()){}

		void AddPoint(std::vector<double>* point, int& NumComp)
		{
			if (IsLeaf())
			{
				Points->push_back(point);
				if (Points->size() > BucketSize && Split != nullptr)
					SplitLeaf(NumComp);
				return;
			}
			if (Split->IsWorse(point, NumComp)) {
				AddPointToNode(point, worseNode, Split->WorseSplit, NumComp);
			}
			else {
				AddPointToNode(point, betterNode, Split->BetterSplit, NumComp);
			}
		}

		bool Dominates(std::vector<double>* s, int& NumComp) const
		{
			int M = s->size();
			for (std::vector<double>* f : *Points)
			{
				if (Dominates(f, s, M))
					return true;
			}
			return false;
		}

	private:
		void SplitLeaf(int& NumComp)
		{
			std::vector<std::vector<double>*> points = *Points;
			Points = nullptr;

			for(std::vector<double>* point : points)
				AddPoint(point, NumComp);
		}

		void AddPointToNode(std::vector<double>* point, std::unique_ptr<NDNode>& node, std::shared_ptr<NDSplit>& split, int& NumComp)
		{
			if (node == nullptr)
				node = std::unique_ptr<NDNode>(new NDNode(split, BucketSize));

			node->AddPoint(point, NumComp);
		}

		bool Dominates(std::vector<double>* a, std::vector<double>* b, int M) const
		{
			bool dominates = false;
			for (int i = 0; i < M; ++i)
			{
				double ai = a->at(i);
				double bi = b->at(i);
				if (bi < ai)
					return false;
				if (ai < bi)
					dominates = true;
			}
			return dominates;
		}
	};
}

#endif // !ENS_NDT_NDNODE_H