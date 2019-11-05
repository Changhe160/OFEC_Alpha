/* -------------------------------ReferencePoint-------------------------------------   
    Reference points play very important roles in NSGA-III. Individuals in the population
    are associated with reference points, and the survivors in the environmental selection
    are determined based on the niche count of the reference points.
   
    Check Algorithms 1-4 in the orignal paper for the usage of reference points.
------------------------------------------------------------------------------------*/

#ifndef OFEC_REF_POINT_H
#define OFEC_REF_POINT_H

#include <vector>
#include <utility>
#include "math_aux.h"

namespace OFEC {
	class ref_point {
	public:
		explicit ref_point(size_t s) : mv_position_(s), m_member_size_(0) {}
		ref_point() {}
		~ref_point();
		const std::vector<real>& pos() const { return mv_position_; }
		std::vector<real>& pos() { return mv_position_; }
		size_t member_size() const { return m_member_size_; }
		bool has_potential_member() const { return !mv_potential_members_.empty(); }
		void clear();
		void add_member();
		void add_potential_member(size_t member_ind, real distance);
		int find_closest_member() const;
		int random_member() const;
		void remove_potential_member(size_t member_ind);
	private:
		std::vector<real> mv_position_;
		// pair<indices of individuals in the population, distance>
		// note. only the data of individuals in the last considered front will be stored.
		std::vector<std::pair<size_t, real> > mv_potential_members_;
		size_t m_member_size_;
	};

	namespace reference_point {

		void generate_recursive(std::vector<ref_point> *rps, ref_point *pt, size_t num_objs, size_t left, size_t total, size_t element);

		// GenerateReferencePoints():
		// Given the number of objectives (M) and the number of divisions (p), generate the set of 
		// reference points. Check Section IV-B and equation (3) in the original paper.
		void generate_ref_points(std::vector<ref_point> *rps, size_t M, const std::vector<size_t> &p);

		// Associate():
		// Associate individuals in the population with reference points.
		// Check Algorithm 3 in the original paper.
		void associate(std::vector<ref_point> *prps, const std::vector<std::vector<real> > &conv_obj, const std::vector<std::vector<int> > &fronts);
	}
}

#endif // !OFEC_REF_POINT_H

