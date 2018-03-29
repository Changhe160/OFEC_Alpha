#ifndef ENS_NDT_NDTREE_H
#define ENS_NDT_NDTREE_H

#include "NDNode.h"

namespace ENS_NDT {
	class NDTree {
	private:
		int Count;
	protected:
		std::unique_ptr<NDNode> root;
	public:
		int get_count() { return Count; }
		NDTree(std::shared_ptr<NDSplit>& split, int bucketSize = 2) : root(new NDNode(split, bucketSize)){}
		
		void Insert(std::vector<double>* point, int& NumComp)
        {
            ++Count;
            root->AddPoint(point, NumComp);
        }

		bool Dominates (std::vector<double>* point, int& NumComp)
		{
			return Dominates(point, root, NumComp);
		}

		bool Dominates (std::vector<double>* point, std::unique_ptr<NDNode>& node, int& NumComp)
		{
			if (node->IsBranch())
			{
				if (node->get_worseNode() != nullptr && node->get_Split()->IsWorse(point, NumComp) && Dominates(point, node->get_worseNode(), NumComp))
					return true;
				return node->get_betterNode() != nullptr && Dominates(point, node->get_betterNode(), NumComp);
			}
			return node->Dominates(point, NumComp);
		}
	};
}

#endif // !ENS_NDT_NDTREE_H