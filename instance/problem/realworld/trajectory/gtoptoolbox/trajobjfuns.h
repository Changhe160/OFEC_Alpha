/*
 *  GOProblems.h
 *  SeGMO, a Sequential Global Multiobjective Optimiser
 *
 *  Created by Dario Izzo on 5/17/08.
 *  Copyright 2008 ¿dvanced Concepts Team (European Space Agency). All rights reserved.
 *
 */

#ifndef GTOP_TOOLBOX_TRAJOBJFUNS_H
#define GTOP_TOOLBOX_TRAJOBJFUNS_H
#include<vector>

namespace gtop_toolbox {

	//NOTE: the functions here have passing by reference + const as they are called a lot of time during execution and thus
	//it is worth trying to save time by avoiding to make a copy of the variable passed

	double gtoc1(const std::vector<double> &x, std::vector<double> &rp);
	double cassini1(const std::vector<double> &x, std::vector<double> &rp);
	double sagas(const std::vector<double> &x, double &DVtot, double &DVonboard);
	double rosetta(const std::vector<double> &x);
	double cassini2(const std::vector<double> &x);
	double messenger(const std::vector<double> &x);
	double messengerfull(const std::vector<double> &x);
	double tandem(const std::vector<double> &x, double &tof, const int sequence_[]);

}

#endif
