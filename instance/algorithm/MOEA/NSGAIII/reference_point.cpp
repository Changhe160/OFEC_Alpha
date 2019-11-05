#include "reference_point.h"
#include <limits>
#include "../../../../core/global.h"

namespace OFEC {
	ref_point::~ref_point() {
		mv_potential_members_.clear();
		mv_position_.clear();
	}

	void ref_point::clear()	{
		m_member_size_ = 0;
		mv_potential_members_.clear();
	}

	void ref_point::add_member() {
		m_member_size_ += 1;
	}

	void ref_point::add_potential_member(size_t member_ind, real distance) {
		mv_potential_members_.push_back(std::move(std::make_pair(member_ind, distance)));
	}

	int ref_point::find_closest_member() const {
		real min_dist = std::numeric_limits<real>::max();
		int min_indv = -1;
		for (size_t i = 0; i < mv_potential_members_.size(); i += 1)
		{
			if (mv_potential_members_[i].second < min_dist)
			{
				min_dist = mv_potential_members_[i].second;
				min_indv = mv_potential_members_[i].first;
			}
		}
		return min_indv;
	}

	int ref_point::random_member() const {
		if (mv_potential_members_.size() > 0)
			return mv_potential_members_[global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, mv_potential_members_.size())].first;
		else
			return -1;
	}

	void ref_point::remove_potential_member(size_t member_ind) {
		for (size_t i = 0; i < mv_potential_members_.size(); i += 1) {
			if (mv_potential_members_[i].first == member_ind) {
				mv_potential_members_.erase(mv_potential_members_.begin() + i);
				return;
			}
		}
	}

	namespace reference_point {
		void generate_recursive(std::vector<ref_point> *rps, ref_point *pt, size_t num_objs, size_t left, size_t total, size_t element) {
			if (element == num_objs - 1) {
				pt->pos()[element] = static_cast<real>(left) / total;
				rps->push_back(*pt);
			}
			else {
				for (size_t i = 0; i <= left; i += 1) {
					pt->pos()[element] = static_cast<real>(i) / total;
					generate_recursive(rps, pt, num_objs, left - i, total, element + 1);
				}
			}
		}

		void generate_ref_points(std::vector<ref_point>* rps, size_t M, const std::vector<size_t>& p) {
			ref_point pt(M);
			generate_recursive(rps, &pt, M, p[0], p[0], (size_t)0);
			if (p.size() > 1) { // two layers of reference points (Check Fig. 4 in NSGA-III paper)
				std::vector<ref_point> inside_rps;
				generate_recursive(&inside_rps, &pt, M, p[1], p[1], (size_t)0);
				real center = 1.0 / M;
				for (size_t i = 0; i < inside_rps.size(); i += 1) {
					for (size_t j = 0; j < inside_rps[i].pos().size(); j += 1)
						inside_rps[i].pos()[j] = (center + inside_rps[i].pos()[j]) / 2; // (k=num_divisions/M, k, k, ..., k) is the center point
					rps->push_back(inside_rps[i]);
				}
			}
		}

		void associate(std::vector<ref_point>* prps, const std::vector<std::vector<real>>& conv_obj, const std::vector<std::vector<int>>& fronts) {
			std::vector<ref_point> &rps = *prps;
			for (size_t t = 0; t < fronts.size(); t += 1) {
				for (size_t i = 0; i < fronts[t].size(); i += 1) {
					size_t min_rp = rps.size();
					real min_dist = std::numeric_limits<real>::max();
					for (size_t r = 0; r < rps.size(); r += 1) {
						real d = math_aux::perpendicular_distance(rps[r].pos(), conv_obj[fronts[t][i]]);
						if (d < min_dist) {
							min_dist = d;
							min_rp = r;
						}
					}
					if (t + 1 != fronts.size()) // associating members in St/Fl (only counting)
						rps[min_rp].add_member();
					else
						rps[min_rp].add_potential_member(fronts[t][i], min_dist);
				} // for - members in front
			} // for - fronts
		}
	}
}