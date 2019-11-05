#include"GrEA.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <algorithm>

namespace OFEC {
	namespace GREA {
		/******************************population*************************/
		GrEA_pop::GrEA_pop(size_t size_pop) : SBX_pop<individual>(size_pop){			
			int M = global::ms_global->m_problem->objective_size();
			m_ind_min_max.resize(M), m_grid_min_max.resize(M), m_grid_distance.resize(M);
		}

		void GrEA_pop::initialize() {
			population::initialize();
		}

		void GrEA_pop::gridConstruct() {
			const int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_ind_min_max[i].second = -1 * 1.0e14;
				m_ind_min_max[i].first = 1.0e14;
				for (int j = 0; j < this->m_inds.size(); ++j) {
					if (this->m_inds[j]->objective(i) > m_ind_min_max[i].second)
						m_ind_min_max[i].second = this->m_inds[j]->objective(i);
					if (this->m_inds[j]->objective(i) < m_ind_min_max[i].first)
						m_ind_min_max[i].first = this->m_inds[j]->objective(i);
				}
				m_grid_distance[i] = (m_ind_min_max[i].second - m_ind_min_max[i].first)*(m_grid_div + 1) / (m_grid_div*m_grid_div);
				m_grid_min_max[i].second = m_ind_min_max[i].second + (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
				m_grid_min_max[i].first = m_ind_min_max[i].first - (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
			}
		}

		void GrEA_pop::gridConstruct_Fi(std::vector<individual> &offspring, std::vector<int>& Fi, int size) {
			const int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_ind_min_max[i].second = -1 * 1.0e14;
				m_ind_min_max[i].first = 1.0e14;
				for (int j = 0; j < size; ++j) {
					if (offspring[Fi[j]].objective()[i] > m_ind_min_max[i].second)
						m_ind_min_max[i].second = offspring[Fi[j]].objective()[i];
					if (offspring[Fi[j]].objective()[i] < m_ind_min_max[i].first)
						m_ind_min_max[i].first = offspring[Fi[j]].objective()[i];
				}
				m_grid_distance[i] = (m_ind_min_max[i].second - m_ind_min_max[i].first)*(m_grid_div + 1) / (m_grid_div*m_grid_div);
				m_grid_min_max[i].second = m_ind_min_max[i].second + (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
				m_grid_min_max[i].first = m_ind_min_max[i].first - (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
			}
		}

		void GrEA_pop::assign_GR_GCPD() {
			const int M = global::ms_global->m_problem->objective_size();
			int flag;
			double value;
			for (int i = 0; i < this->m_inds.size(); i++) {
				flag = 0;
				value = 0;
				for (int j = 0; j < M; j++) {
					*this->m_inds[i]->Gk(j) = (int)floor((this->m_inds[i]->objective(j) - m_grid_min_max[j].first) / m_grid_distance[j]);
					flag += *this->m_inds[i]->Gk(j);
					value += pow((this->m_inds[i]->objective(j) - (m_grid_min_max[j].first + *this->m_inds[i]->Gk(j) * m_grid_distance[j])) / m_grid_distance[j], 2.0);
				}
				this->m_inds[i]->GR() = flag;
				this->m_inds[i]->GCPD() = sqrt(value);
			}
		}

		void GrEA_pop::assign_GR_GCPD_Fi(std::vector<individual> &offspring, std::vector<int>& Fi, int size) {
			const int M = global::ms_global->m_problem->objective_size();
			int flag;
			double value;
			for (int i = 0; i < size; i++) {
				flag = 0;
				value = 0;
				for (int j = 0; j < M; j++) {
					*offspring[Fi[i]].Gk(j) = (int)floor((offspring[Fi[i]].objective()[j] - m_grid_min_max[j].first) / m_grid_distance[j]);
					flag += *offspring[Fi[i]].Gk(j);
					value += pow((offspring[Fi[i]].objective()[j] - (m_grid_min_max[j].first + *offspring[Fi[i]].Gk(j) * m_grid_distance[j])) / m_grid_distance[j], 2.0);
				}
				offspring[Fi[i]].GR() = flag;
				offspring[Fi[i]].GCPD() = sqrt(value);
			}
		}

		void GrEA_pop::assign_GCD() {
			int flag;
			const int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < this->m_inds.size(); i++) {
				for (int j = i + 1; j < this->m_inds.size(); j++) {
					flag = 0;
					for (int k = 0; k < M; k++)
						flag += abs(*this->m_inds[i]->Gk(k) - *this->m_inds[j]->Gk(k));
					if (flag < M) {
						this->m_inds[i]->GCD() += M - flag;
						this->m_inds[j]->GCD() += M - flag;
					}
				}
			}
		}
				
		int GrEA_pop::check_dominance_grid(individual &a, individual &b) {
			int M = global::ms_global->m_problem->objective_size();
			int flag1 = 0, flag2 = 0;
			for (int i = 0; i < M; ++i) {
				if (a.Gk(i) < b.Gk(i))
					flag1 = 1;
				else if (a.Gk(i) > b.Gk(i))
					flag2 = 1;
			}
			if (flag1 == 1 && flag2 == 0)
				return 1;
			else if (flag1 == 0 && flag2 == 1)
				return -1;
			else
				return 0;
		}

		void GrEA_pop::eval_dens() {      //environment selection
			int M = global::ms_global->m_problem->objective_size();
			int pops = 0;  //indicate parent population size be 0
			int size = m_offspring.size();
			int rank = 0;
			while (true) {
				int count = 0;
				for (size_t i = 0; i < size; i++)
					if (m_offspring[i].rank() == rank)
						count++;
				int size2 = pops + count;
				if (size2 > this->m_inds.size()) {
					break;
				}
				for (size_t i = 0; i < size; i++)
					if (m_offspring[i].rank() == rank){
						*(this->m_inds[pops]) = m_offspring[i];
						++pops;
					}
				rank++;
				if (pops >= this->m_inds.size()) break;
			}
			if (pops < this->m_inds.size())
				return;
			else {
				//get all individual of in Fi
				std::vector<int> Fi;
				int Fi_size = 0;
				for (size_t i = 0; i < size; ++i) {
					if (m_offspring[i].rank() == rank) {
						Fi.push_back(i);
						Fi_size++;
					}
				}
				//grid construct of Fi
				gridConstruct_Fi(m_offspring, Fi, Fi_size);
				//assign fitness of GR and GCPD
				assign_GR_GCPD_Fi(m_offspring, Fi, Fi_size);
				for (int j = 0; j < Fi_size; ++j) {
					m_offspring[Fi[j]].GCD() = 0;
				}

				while (pops < this->m_inds.size()) {
					//******************find best solution (q) from, algorithm 7
					int q = 0;
					for (int k = 1; k < Fi_size; ++k) {
						if (m_offspring[Fi[k]].GR() < m_offspring[Fi[q]].GR())
							q = k;
						else if (m_offspring[Fi[k]].GR() == m_offspring[Fi[q]].GR()) {
							if (m_offspring[Fi[k]].GCD() < m_offspring[Fi[q]].GCD())
								q = k;
							else if (m_offspring[Fi[k]].GCD() == m_offspring[Fi[q]].GCD())
								if (m_offspring[Fi[k]].GCPD() < m_offspring[Fi[q]].GCPD())
									q = k;
						}
					}
					*(this->m_inds[pops++]) = m_offspring[Fi[q]];
					int Fi_q = Fi[q];
					Fi.erase(Fi.begin() + q);
					Fi_size--;
					//******************compute GCD of Fi, algorithm 6
					int flag;
					for (int m = 0; m < Fi_size; ++m) {
						for (int n = m + 1; n < Fi_size; ++n) {
							flag = 0;
							for (int s = 0; s < M; ++s)
								flag += abs(*(m_offspring[Fi[m]].Gk(s)) - *(m_offspring[Fi[n]].Gk(s)));
							if (flag < M) {
								m_offspring[Fi[m]].GCD() = M - flag;
								m_offspring[Fi[n]].GCD() = M - flag;
							}
						}
					}
					//******************adjust GR of individua in Fi, aigorithm 3
					//equal, dominated, non dominate, non equal
					int GD_q_p;
					std::vector<int> PD(Fi_size, 0);
					for (int m = 0; m < Fi_size; ++m) {
						GD_q_p = 0;
						for (int n = 0; n < M; ++n) {
							GD_q_p += abs(*(m_offspring[Fi_q].Gk(n)) - *(m_offspring[Fi[q]].Gk(m)));
						}
						if (GD_q_p == 0)
							m_offspring[Fi[m]].GR() = M + 2;
						else if (check_dominance_grid(m_offspring[Fi_q], m_offspring[Fi[m]]) == 1)
							m_offspring[Fi[m]].GR() = M;
						else {
							if (GD_q_p < M) {
								if (PD[m] < (M - GD_q_p)) {
									PD[m] = M - GD_q_p;
									for (int n = 0; n < Fi_size; ++n)
										if ((m != n) && (check_dominance_grid(m_offspring[Fi[m]], m_offspring[Fi[n]]) == 1) && (PD[n] < PD[m]))
											PD[n] = PD[m];
								}
							}
							m_offspring[Fi[m]].GR() = m_offspring[Fi_q].GR() + PD[m];
						}
					}
				}
			}
		}

		evaluation_tag GrEA_pop::evolve_mo() {			
			if (m_inds.size() % 2 != 0)
				throw myexcept("population size should be even @NSGAII_SBXRealMu::evolve_mo()");
			evaluation_tag tag = evaluation_tag::Normal;
			int m = 0;
			for (size_t n = 0; n < m_inds.size(); n += 2) {
				std::vector<int> p(2);				
				p[0] = this->tournament_selection();
				do { p[1] = this->tournament_selection(); } while (p[1] == p[0]);
				crossover(p[0], p[1], m_offspring[m], m_offspring[m + 1]);
				mutate(m_offspring[m]);
				mutate(m_offspring[m + 1]);
				tag = m_offspring[m].evaluate();
				if (tag != evaluation_tag::Normal) break;
				tag = m_offspring[m + 1].evaluate();
				if (tag != evaluation_tag::Normal) break;
				m += 2;
				m_offspring[m++] = *m_inds[n];
				m_offspring[m++] = *m_inds[n + 1];
			}
			return tag;
		}

		evaluation_tag GrEA_pop::evolve() {
			//std::cout << "the " << this->m_iter << " generation" << std::endl;
			evaluation_tag tag = evaluation_tag::Normal;
			tag = evolve_mo();		
			nondominated_sorting(m_offspring);
			eval_dens();
			this->m_iter++;
			return tag;
		}
				

		/**********************GrEA************************/		
		GrEA::GrEA(param_map & v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {}

		void GrEA::initialize() {
			m_pop.set_cr(0.9);
			m_pop.set_mr(1.0 / CONTINUOUS_CAST->variable_size());
			m_pop.set_eta(20, 20);			
			m_pop.initialize();
			m_pop.evaluate();
			m_pop.gridConstruct();
			m_pop.assign_GR_GCPD();
			m_pop.assign_GCD();
		}

		void GrEA::run_() {
			while (!terminating()) {
				m_pop.evolve();
			}
		}

		void GrEA::record() {
			size_t evals = CONTINUOUS_CAST->evaluations();
			real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
			measure::get_measure()->record(global::ms_global.get(), evals, IGD);
		}

	}
}