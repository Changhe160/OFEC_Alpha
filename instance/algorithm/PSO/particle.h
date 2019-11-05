/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 21 Sep. 2011 by Changhe Li
// Updated: 12 Dec. 2014 by Changhe Li
// Updated��15 Mar. 2018 by Junchen Wang (wangjunchen@cug.edu.cn)
// Updated: 25.Aug. 2019 Changhe Li

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../../../core/algorithm/individual.h"

namespace OFEC {
	class particle : public individual<> {
	protected:
		solution<> m_pbest;
		std::vector<real> m_vel;
	public:
		particle() = default;
		particle(size_t num_obj, size_t num_con, size_t size_var);
		particle(const particle& rhs) = default;
        particle(particle &&rhs) noexcept = default;
        particle& operator=(const particle &other) = default;
        particle& operator=(particle &&other) noexcept = default;
		real velocity();
		virtual void initialize_velocity();
		void initialize_velocity(const std::vector<std::pair<real, real>> &vrange);
		void initialize_velocity(real min, real max);
		virtual void next_velocity(const solution<> &lbest, real w, real c1, real c2);
		virtual void clamp_velocity(); // set to zero by default if the particle goes out the space
		void move();		 
		void resize_variable(size_t n);
		const std::vector<real>& get_vel() const { return m_vel; }
		solution<>& pbest() { return m_pbest; }
		const solution<>& pbest()const { return m_pbest; }
	};
}

#endif // !PARTICLE_H

