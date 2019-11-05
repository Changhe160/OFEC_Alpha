/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia & Junchen Wang 
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 11 Jan 2015 by Changhe Li
// Last modified: 15 Aug 2019 by Junchen Wang (Email: wangjunchen.chris@gmail.com) 

/* --------------------------------NSGAIII-------------------------------------------      
 Deb and Jain, "An Evolutionary Many-Objective Optimization Algorithm Using
 Reference-point Based Non-dominated Sorting Approach, Part I: Solving Problems with
 Box Constraints," IEEE Transactions on Evolutionary Computation, to appear.
 http://dx.doi.org/10.1109/TEVC.2013.2281535
 ---------------------------------------------------------------------------------- */

#ifndef OFEC_NSGAIII_H
#define OFEC_NSGAIII_H

#include "../../../../utility/nondominated_sorting/fast_sort.h"
#include "reference_point.h"
#include <algorithm>

namespace OFEC {
	template<typename Individual>
	class NSGAIII {
	public:
		using objective_type = typename Individual::solution_type::objective_encoding;
	public:
		NSGAIII(size_t size_pop) {
			size_t num_obj = global::ms_global->m_problem->objective_size();
			set_default_param();
			reference_point::generate_ref_points(&mv_rps, num_obj, mv_obj_division_p_);
			mvv_off_conv_obj.resize(2 * size_pop);
			for (int i = 0; i < 2 * size_pop; i++)
				mvv_off_conv_obj[i].resize(num_obj);
		}
		void survivor_selection(std::vector<std::unique_ptr<Individual>>& parent, std::vector<Individual>& offspring);
	private:
		void set_default_param();
		void nondominated_sorting(std::vector<Individual>& offspring);
		std::vector<real> translate_objectives(const std::vector<std::vector<int> > &fronts, std::vector<Individual>& offspring);
		void find_extreme_points(std::vector<size_t> *extreme_points, const std::vector<std::vector<int> > &fronts, std::vector<Individual>& offspring);
		void construct_hyperplane(std::vector<real> *pintercepts, const std::vector<size_t> &extreme_points, std::vector<Individual>& offspring);
		void normalize_objectives(const std::vector<std::vector<int> > &fronts, const std::vector<real> &intercepts, const std::vector<real> &ideal_point);
		size_t find_niche_ref_point(const std::vector<ref_point> &rps);
		int select_cluster_member(const ref_point &rp);
	protected:
		std::vector<size_t> mv_obj_division_p_;
		std::vector<ref_point> mv_rps;
		std::vector<std::vector<real>> mvv_off_conv_obj;
	};

	template<typename Individual>
	void NSGAIII<Individual>::set_default_param() {
		if (global::ms_global->m_problem->name().find("DTLZ") != std::string::npos) {
			int num_obj = global::ms_global->m_problem->objective_size();
			if (num_obj == 3)
				mv_obj_division_p_.resize(1, 12);
			else if (num_obj == 5)
				mv_obj_division_p_.resize(1, 6);
			else if (num_obj == 8)
			{
				mv_obj_division_p_.resize(2);
				mv_obj_division_p_[0] = 3;
				mv_obj_division_p_[1] = 2;
			}
			return;
		}
		mv_obj_division_p_.resize(1, 12);
	}

	template<typename Individual>
	void NSGAIII<Individual>::nondominated_sorting(std::vector<Individual>& offspring) {
		std::vector<std::vector<objective_type>*> objs;
		for (auto& i : offspring)
			objs.emplace_back(&i.objective());
		std::vector<int> rank;
		NS::fast_sort<objective_type>(objs, rank, global::ms_global->m_problem->opt_mode());
		for (size_t i = 0; i < offspring.size(); ++i)
			offspring[i].set_rank(rank[i]);
	}

	template<typename Individual>
	void NSGAIII<Individual>::survivor_selection(std::vector<std::unique_ptr<Individual>>& parent, std::vector<Individual>& offspring) {
		std::vector<ref_point> rps = mv_rps;
		std::vector<std::vector<int> > fronts;
		int rank = 0;
		int count = 0;
		int size = offspring.size();
		// ---------- Step 4 in Algorithm 1: non-dominated sorting ----------
		nondominated_sorting(offspring);
		while (1)
		{
			std::vector<int> temp;
			for (int i = 0; i < size; i++)
			{
				if (offspring[i].rank() == rank)
				{
					temp.push_back(i);
					++count;
				}
			}
			fronts.push_back(temp);
			if (count == size) break;
			++rank;
		}

		// ---------- Steps 5-7 in Algorithm 1 ----------
		std::vector<size_t> considered; // St
		int last = 0, next_size = 0;
		while (next_size < parent.size())
		{
			next_size += fronts[last].size();
			last += 1;
		}
		fronts.erase(fronts.begin() + last, fronts.end()); // remove useless individuals

		count = 0;
		for (size_t t = 0; t < fronts.size() - 1; t += 1)
			for (size_t i = 0; i < fronts[t].size(); i += 1)
				*(parent[count++]) = offspring[fronts[t][i]];

		// ---------- Steps 9-10 in Algorithm 1 ----------
		if (count == parent.size()) return;


		// ---------- Step 14 / Algorithm 2 ----------
		std::vector<real> ideal_point = translate_objectives(fronts, offspring);

		std::vector<size_t> extreme_points;
		find_extreme_points(&extreme_points, fronts, offspring);

		std::vector<real> intercepts;
		construct_hyperplane(&intercepts, extreme_points, offspring);

		normalize_objectives(fronts, intercepts, ideal_point);

		// ---------- Step 15 / Algorithm 3, Step 16 ----------
		reference_point::associate(&rps, mvv_off_conv_obj, fronts);

		// ---------- Step 17 / Algorithm 4 ----------
		while (count < parent.size())
		{
			size_t min_rp = find_niche_ref_point(rps);

			int chosen = select_cluster_member(rps[min_rp]);
			if (chosen < 0) // no potential member in Fl, disregard this reference point
			{
				rps.erase(rps.begin() + min_rp);
			}
			else
			{
				rps[min_rp].add_member();
				rps[min_rp].remove_potential_member(chosen);
				*(parent[count++]) = offspring[chosen];
			}
		}
	}

	template<typename Individual>
	std::vector<real> NSGAIII<Individual>::translate_objectives(const std::vector<std::vector<int>>& fronts, std::vector<Individual>& offspring) {
		int numObj = global::ms_global->m_problem->objective_size();
		std::vector<real> ideal_point(numObj);

		for (int f = 0; f < numObj; f += 1)
		{
			real minf = std::numeric_limits<real>::max();
			for (size_t i = 0; i < fronts[0].size(); i += 1) // min values must appear in the first front
			{
				minf = std::min(minf, offspring[fronts[0][i]].objective()[f]);
			}
			ideal_point[f] = minf;

			for (size_t t = 0; t < fronts.size(); t += 1)
			{
				for (size_t i = 0; i < fronts[t].size(); i += 1)
				{
					size_t ind = fronts[t][i];
					mvv_off_conv_obj[ind][f] = offspring[ind].objective()[f] - minf;
				}
			}
		}

		return ideal_point;
	}

	template<typename Individual>
	void NSGAIII<Individual>::find_extreme_points(std::vector<size_t>* extreme_points, const std::vector<std::vector<int>>& fronts, std::vector<Individual>& offspring) {
		int numObj = global::ms_global->m_problem->objective_size();
		std::vector<size_t> &exp = *extreme_points;
		exp.clear();

		for (size_t f = 0; f < numObj; f += 1)
		{
			std::vector<real> w(numObj, 0.000001);
			w[f] = 1.0;

			real min_ASF = std::numeric_limits<real>::max();
			size_t min_indv = fronts[0].size();

			for (size_t i = 0; i < fronts[0].size(); i += 1)  // only consider the individuals in the first front
			{
				real asf = math_aux::ASF(offspring[fronts[0][i]].objective(), w);
				if (asf < min_ASF)
				{
					min_ASF = asf;
					min_indv = fronts[0][i];
				}
			}

			exp.push_back(min_indv);
		}
	}

	template<typename Individual>
	void NSGAIII<Individual>::construct_hyperplane(std::vector<real>* pintercepts, const std::vector<size_t>& extreme_points, std::vector<Individual>& offspring) {
		// Check whether there are duplicate extreme points.
	// This might happen but the original paper does not mention how to deal with it.
		int numObj = global::ms_global->m_problem->objective_size();
		bool duplicate = false;
		for (size_t i = 0; !duplicate && i < extreme_points.size(); i += 1)
		{
			for (size_t j = i + 1; !duplicate && j < extreme_points.size(); j += 1)
			{
				duplicate = (extreme_points[i] == extreme_points[j]);
			}
		}

		std::vector<real> &intercepts = *pintercepts;
		intercepts.assign(numObj, 0);

		if (duplicate) // cannot construct the unique hyperplane (this is a casual method to deal with the condition)
		{
			for (size_t f = 0; f < intercepts.size(); f += 1)
			{
				// extreme_points[f] stands for the individual with the largest value of objective f
				intercepts[f] = offspring[extreme_points[f]].objective()[f];
			}
		}
		else
		{
			// Find the equation of the hyperplane
			std::vector<real> b(numObj, 1.0);
			std::vector<std::vector<real>> A;
			for (size_t p = 0; p < extreme_points.size(); p += 1)
			{
				A.push_back(offspring[extreme_points[p]].objective());
			}
			std::vector<real> x;
			math_aux::guassian_elimination(&x, A, b);

			// Find intercepts
			for (size_t f = 0; f < intercepts.size(); f += 1)
			{
				intercepts[f] = 1.0 / x[f];
			}
		}
	}

	template<typename Individual>
	void NSGAIII<Individual>::normalize_objectives(const std::vector<std::vector<int>>& fronts, const std::vector<real>& intercepts, const std::vector<real>& ideal_point) {
		int numObj = global::ms_global->m_problem->objective_size();
		for (size_t t = 0; t < fronts.size(); t += 1)
		{
			for (size_t i = 0; i < fronts[t].size(); i += 1)
			{
				size_t ind = fronts[t][i];
				for (size_t f = 0; f < numObj; f += 1)
				{
					if (fabs(intercepts[f] - ideal_point[f]) > 10e-10) // avoid the divide-by-zero error
						mvv_off_conv_obj[ind][f] = mvv_off_conv_obj[ind][f] / (intercepts[f] - ideal_point[f]);
					else
						mvv_off_conv_obj[ind][f] = mvv_off_conv_obj[ind][f] / 10e-10;
				}
			}
		}
	}
	template<typename Individual>
	size_t NSGAIII<Individual>::find_niche_ref_point(const std::vector<ref_point>& rps) {
		// find the minimal cluster size
		size_t min_size = std::numeric_limits<size_t>::max();
		for (size_t r = 0; r < rps.size(); r += 1)
		{
			min_size = std::min(min_size, rps[r].member_size());
		}

		// find the reference points with the minimal cluster size Jmin
		std::vector<size_t> min_rps;
		for (size_t r = 0; r < rps.size(); r += 1)
		{
			if (rps[r].member_size() == min_size)
			{
				min_rps.push_back(r);
			}
		}

		// return a random reference point (j-bar)
		int rnd = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, min_rps.size());
		return min_rps[rnd];
	}

	template<typename Individual>
	int NSGAIII<Individual>::select_cluster_member(const ref_point & rp) {
		int chosen = -1;
		if (rp.has_potential_member())
		{
			if (rp.member_size() == 0) // currently has no member
			{
				chosen = rp.find_closest_member();
			}
			else
			{
				chosen = rp.random_member();
			}
		}
		return chosen;
	}
}



#endif // !OFEC_NSGAIII_H
