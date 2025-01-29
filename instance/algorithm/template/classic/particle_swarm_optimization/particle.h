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

/******************************************************************************
* Created: 21 Sep. 2011 by Changhe Li
* Last modified: 8 Apr. 2022 by Junchen Wang
*********************************************************************************/

#ifndef OFEC_PARTICLE_H
#define OFEC_PARTICLE_H

#include "../../../../../core/problem/solution.h"

namespace ofec {
	class Particle : public Solution<> {
		struct VelMax {
			double min, max;
		};
	protected:
		Solution<> m_pbest;
		std::vector<Real> m_vel;
		std::vector<VelMax> m_vel_max;
		bool m_improved;
	public:
		Particle() = default;
		Particle(Environment *env);
		Particle(size_t num_obj, size_t num_con, size_t size_var);
		explicit Particle(const Solution<> &sol);

		std::vector<Real>& velocity() { return m_vel; }
		Solution<>& pbest() { return m_pbest; }
		virtual void initVelocityMax(Environment *env, Random *rnd);
		virtual void initVelocity(Environment *env, Random *rnd);
		void initVelocity(const std::vector<std::pair<Real, Real>> &vrange, Random *rnd);
		void initVelocity(Real min, Real max, Random *rnd);
		virtual void nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, Random *rnd);
		virtual void clampVelocity(Environment *env, Random *rnd); // set to zero by default if the particle goes out the space
		virtual void move();
		virtual void updatePBest(Environment *env);
		void resizeVar(size_t n);
		void setImproved(bool flag) { m_improved = flag; }

		Real speed() const;
		const std::vector<Real>& velocity() const { return m_vel; }
		const Solution<>& pbest() const { return m_pbest; }
		bool isImproved() const { return m_improved; }
	};
}

#endif // !OFEC_PARTICLE_H

