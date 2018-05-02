/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 29 Mar 2018 by Junchen Wang
// Last modified:


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

		bool Dominates(std::vector<double>* point, int& NumComp)
		{
			return Dominates(point, root, NumComp);
		}

		bool Dominates(std::vector<double>* point, std::unique_ptr<NDNode>& node, int& NumComp)
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