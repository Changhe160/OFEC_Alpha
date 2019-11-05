/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class swarm for particle swarm optimization
*
*
*********************************************************************************/
// Created: 21 Sep. 2011 by Changhe Li
// Updated: 12 Dec. 2014 by Changhe Li
// Updated  15 Mar. 2018 by Junchen Wang (wangjunchen@cug.edu.cn)
// Updated: 1 Sep. 2018 by Changhe Li
// Updated: 25.Aug. 2019 Changhe Li

#ifndef SWARM_H
#define SWARM_H

#include "../../../core/algorithm/population.h"
#include <numeric>

namespace OFEC {
	template <typename Particle>
	class swarm : public population<Particle> {
	protected:			
		real m_C1 = 0, m_C2 = 0;						//accelerators
		real m_weight = 0;								// inertia weight		
		std::vector<std::vector<bool>> m_link;			//population topology
		bool m_flag_best_impr = false;
	public:
		swarm(size_t size_pop);
		void initialize() override;
		void initialize_pbest();
		evaluation_tag evolve() override;
		real& weight();		
		real& accelerator1();
		real& accelerator2();
		real average_velocity();
		virtual void set_neighborhood(); //  The adaptive random topology by default
		virtual solution<>& neighborhood_best(int);
	};
	
	template<typename Particle>
	swarm<Particle>::swarm(size_t size_pop) : population<Particle>(size_pop, global::ms_global->m_problem->variable_size()), 
		m_link(size_pop,std::vector<bool>(size_pop)){
	}

	template<typename Particle>
	void swarm<Particle>::initialize_pbest() {
		for (auto& i : this->m_inds)
			i->pbest() = i->solut();
		this->update_best();
	}

	template<typename Particle>
	void swarm<Particle>::initialize() {
		population<Particle>::initialize();
		for (auto& i : this->m_inds)
			i->initialize_velocity();
	}

	template<typename Particle>
	evaluation_tag swarm<Particle>::evolve()	{
		evaluation_tag rf = evaluation_tag::Normal;

		//generate a permutation of particle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		global::ms_global->m_uniform[caller::Algorithm]->shuffle(rindex.begin(), rindex.end());

		this->set_neighborhood();

		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhood_best(rindex[i]);

			this->m_inds[rindex[i]]->next_velocity(x, m_weight, m_C1, m_C2);
			this->m_inds[rindex[i]]->move();
			this->m_inds[rindex[i]]->clamp_velocity();

			rf = this->m_inds[rindex[i]]->evaluate();

			if (this->m_inds[rindex[i]]->dominate(this->m_inds[rindex[i]]->pbest())) {
				this->m_inds[rindex[i]]->pbest() = this->m_inds[rindex[i]]->solut();
				if (this->update_best(this->m_inds[rindex[i]]->solut()))
					m_flag_best_impr = true;
				this->m_best_updated = true;
			}

			this->handle_evaluation_tag(rf);
			if (rf != evaluation_tag::Normal) break;
		}
		this->m_iter++;
		return rf;
	}

	template<typename Particle>
	real& swarm<Particle>::weight() {
		return m_weight ;
	}
	template<typename Particle>
	real& swarm<Particle>::accelerator1() {
		return m_C1;
		
	}
	template<typename Particle>
	real& swarm<Particle>::accelerator2() {
		return m_C2;
	}

	template<typename Particle>
	real swarm<Particle>::average_velocity()
	{
		real avg = 0;		
		for (int i = 0; i<this->m_inds.size(); ++i) 
			avg += this->m_inds[i]->velocity();
		return avg>0?avg / this->m_inds.size():0;
	}

	template<typename Particle>
	solution<>& swarm<Particle>::neighborhood_best(int idx){
		int l = idx;
		for (int i = 0; i < this->m_inds.size();++i) {
			if (m_link[idx][i]&&this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest())) {
				l = i;
			}
		}
		return this->m_inds[l]->pbest();
	}

	template<typename Particle>
	void swarm<Particle>::set_neighborhood() {
		if (!m_flag_best_impr) {
			real p = 1 - pow(1 - 1. / this->m_inds.size(), 3);
			for (int i = 0; i<this->m_inds.size(); i++) {
				for (int k = 0; k<this->m_inds.size(); k++) {
					if (k == i || global::ms_global->m_uniform[caller::Algorithm]->next() < p)
						m_link[i][k] = true; //probabilistic method
					else
						m_link[i][k] = false;
				}
			}
		}
	}

}
#endif // !SWARM_H


