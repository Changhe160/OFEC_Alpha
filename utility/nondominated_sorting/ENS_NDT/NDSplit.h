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


#ifndef ENS_NDT_NDSPLIT_H
#define ENS_NDT_NDSPLIT_H

#include "../quick_sort.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace ENS_NDT {
	class NDSplit {
	public:
		int Dimensions;
		int SplitDimension;
		double SplitValue;
		std::shared_ptr<NDSplit> WorseSplit = nullptr;
		std::shared_ptr<NDSplit> BetterSplit = nullptr;

	public:
		bool IsWorse(std::vector<double>* point, int& NumComp)
		{
			NumComp++;
			return point->at(SplitDimension) >= SplitValue;
		}

		bool IsBetter(std::vector<double>* point, int& NumComp)
		{
			return !IsWorse(point, NumComp);
		}

		static std::shared_ptr<NDSplit> GenerateSplits(std::vector<std::vector<double>>& points, int dimensions, int& NumComp, int bucketSize = 2)
		{
			return GenerateSplits(points, 0, dimensions, bucketSize, 0, points.size(), NumComp);
		}

	private:
		static std::shared_ptr<NDSplit> GenerateSplits(std::vector<std::vector<double>>& points, int dimension, int dimensions, int bucketSize, int startIndex, int count, int& NumComp)
		{
			if (count <= bucketSize)
				return nullptr;

			std::vector<int>* temp_index = new std::vector<int>;
			std::vector<std::vector<double>>* temp_points = new std::vector<std::vector<double>>;
			NumComp += NS::quick_sort(points, *temp_index, dimension, startIndex, startIndex + count - 1);
			temp_points->assign(points.begin() + startIndex, points.begin() + startIndex + count);
			for (int i = 0; i < count; ++i)
				points[i + startIndex] = temp_points->at(temp_index->at(i)-startIndex);
			delete temp_index;
			delete temp_points;

			int betterCount = (count + 1) / 2;
			int worseCount = count / 2;
			int startIndex2 = startIndex + betterCount;
			int nextDimension = (dimension + 1) % dimensions;

			std::shared_ptr<NDSplit> split(new NDSplit());
			split->Dimensions = dimensions;
			split->SplitDimension = dimension;
			split->SplitValue = points[startIndex2][dimension];
			split->BetterSplit = GenerateSplits(points, nextDimension, dimensions, bucketSize, startIndex, betterCount, NumComp);
			split->WorseSplit = GenerateSplits(points, nextDimension, dimensions, bucketSize, startIndex2, worseCount, NumComp);

			return split;
		}
	};
}

#endif // !ENS_NDT_NDSPLIT_H