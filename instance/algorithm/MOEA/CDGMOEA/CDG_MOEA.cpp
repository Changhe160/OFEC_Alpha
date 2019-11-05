#include<fstream>
#include<iomanip>
#include "CDG_MOEA.h"
#include "../../../../utility/nondominated_sorting/fast_sort.h"

namespace OFEC {
	namespace CDG_MOEA {
		/******************************CDGMOEA_pop***********************************/
		CDGMOEA_pop::CDGMOEA_pop(size_t size_pop) :MOEA_DE_pop(size_pop) {
			m_offspring.resize(2 * size_pop);
			int M = global::ms_global->m_problem->objective_size();
			if (M == 2) {
				m_grid_div = 180;
				m_T = 5;
			}
			else if (M == 3) {
				m_grid_div = 30;
				m_T = 1;
			}
			m_ideal.resize(M), m_nadir.resize(M), m_grid_distance.resize(M), m_grid_min_max.resize(M),
				m_S.resize(M, std::vector<std::vector<int>>(pow(m_grid_div, M - 1)));// m_R.resize(2 * size_pop, std::vector<int>(M));
			for (int i = 0; i < M; ++i) {
				m_ideal[i] = m_nadir[i] = this->m_inds[0]->objective()[i];
			}
		}

		void CDGMOEA_pop::initialize() {
			MOEA_DE_pop::initialize();						
		}

		void CDGMOEA_pop::update_ideal_point() {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_ideal[i] = 1.0e14;
				for (int j = 0; j < this->m_inds.size(); ++j) {
					if (this->m_inds[j]->objective()[i] < m_ideal[i])
						m_ideal[i] = this->m_inds[j]->objective()[i];
				}
			}
		}

		void CDGMOEA_pop::update_ideal_point(const std::vector<individual> &offspring) {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {				
				for (int j = 0; j < offspring.size(); ++j) {
					if (offspring[j].objective()[i] < m_ideal[i])
						m_ideal[i] = offspring[j].objective()[i];
				}
			}
		}

		void CDGMOEA_pop::update_nadir_point() {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_nadir[i] = -1 * 1.0e14;
				for (int j = 0; j < this->m_inds.size(); ++j) {
					if (m_nadir[i] < this->m_inds[j]->objective()[i])
						m_nadir[i] = this->m_inds[j]->objective()[i];
				}
			}
		}

		void CDGMOEA_pop::update_nadir_point(const std::vector<individual> &offspring) {  //
			int M = global::ms_global->m_problem->objective_size();
			std::vector<int> SP;
			for (int i = 0; i < offspring.size(); ++i) {
				int flag = 1;
				for (int j = 0; j < M; ++j) {
					if (offspring[i].objective()[j] < (m_ideal[j] + m_nadir[j] / 5)) {
						SP.push_back(i);
						break;
					}						
				}				
			}
			if (SP.size() != 0) {	//find all nondominated solution
				std::vector<individual> SP1(SP.size());
				for (int i = 0; i < SP.size(); ++i) {
					SP1[i] = offspring[SP[i]];
				}
				nondominated_sorting(SP1);								
				int size = SP.size(), count = 0;
				std::vector<int>().swap(SP);	//free  SP
				for (int i = 0; i < size; i++) {
					if (SP1[i].rank() == 0) {
						count++;
						SP.push_back(i);
					}						
				}
				for (int i = 0; i < M; ++i) {
					m_nadir[i] = -1 * 1.0e14;
					for (int j = 0; j < SP.size(); ++j) {
						if (m_nadir[i] < SP1[SP[j]].objective()[i])
							m_nadir[i] = SP1[SP[j]].objective()[i];
					}
				}
			}
			
		}

		void CDGMOEA_pop::gridConstruct() {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_grid_distance[i] = (m_nadir[i] - m_ideal[i])*(m_grid_div + 1) / (m_grid_div*m_grid_div);
				m_grid_min_max[i].first = m_ideal[i] - (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
				m_grid_min_max[i].second = m_nadir[i] + (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
			}
		}

		void CDGMOEA_pop::assign_GN() {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < this->m_inds.size(); i++) {
				for (int j = 0; j < M; j++) {
					this->m_inds[i]->Gk()[j] = (int)ceil((this->m_inds[i]->objective(j) - m_grid_min_max[j].first) / m_grid_distance[j]);
				}
			}
			for (int i = 0; i < this->m_inds.size(); ++i) {
				for (int j = i + 1; j < this->m_inds.size(); ++j) {
					int GD_max = abs(this->m_inds[i]->Gk()[0] - this->m_inds[j]->Gk()[0]);
					for (int k = 0; k < M; ++k) {
						if (GD_max < abs(this->m_inds[i]->Gk()[k] - this->m_inds[j]->Gk()[k]))
							GD_max = abs(this->m_inds[i]->Gk()[k] - this->m_inds[j]->Gk()[k]);
					}
					if (GD_max < m_T || GD_max == m_T) {
						this->m_inds[i]->GN().push_back(j);
						this->m_inds[j]->GN().push_back(i);
					}
				}
			}
		}

		void CDGMOEA_pop::gridConstruct_assignGN_P_reserve(std::vector<individual> &offspring, std::vector<int>& P_reserve, int size) {
			int M = global::ms_global->m_problem->objective_size();
			for (int i = 0; i < M; ++i) {
				m_grid_distance[i] = (m_nadir[i] - m_ideal[i])*(m_grid_div + 1) / (m_grid_div*m_grid_div);
				m_grid_min_max[i].first = m_ideal[i] - (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
				m_grid_min_max[i].second = m_nadir[i] + (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
			}
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < M; j++) {
					offspring[P_reserve[i]].Gk()[j] = (int)ceil((offspring[P_reserve[i]].objective()[j] - m_grid_min_max[j].first) / m_grid_distance[j]);
				}
			}
			for (int i = 0; i < size; ++i) {
				for (int j = i + 1; j < size; ++j) {
					int GD_max = abs(offspring[P_reserve[i]].Gk()[0] - offspring[P_reserve[j]].Gk()[0]);
					for (int k = 0; k < M; ++k) {
						if (GD_max < abs(offspring[P_reserve[i]].Gk()[k] - offspring[P_reserve[j]].Gk()[k]))
							GD_max = abs(offspring[P_reserve[i]].Gk()[k] - offspring[P_reserve[i]].Gk()[k]);
					}
					if (GD_max < m_T || GD_max == m_T) {
						offspring[P_reserve[i]].GN().push_back(P_reserve[j]);
						offspring[P_reserve[j]].GN().push_back(P_reserve[i]);
					}
				}
			}

		}

		void CDGMOEA_pop::assign_S(std::vector<individual> &offspring, std::vector<int> P_reserve) {
			int M = global::ms_global->m_problem->objective_size();
			int size = pow(m_grid_div, M - 1);
			if (M == 2) {
				for (int i = 0; i < P_reserve.size(); ++i) {
					for (int l = 0; l < M; ++l) {
						for (int j = 0; j < M; ++j) {
							if (j != l) {
								for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)							
									if (offspring[P_reserve[i]].Gk()[j] == k + 1)
										m_S[l][k].push_back(P_reserve[i]);
								}
							}
						}
					}
				}
			}
			else if (M == 3) {
				for (int i = 0; i < P_reserve.size(); ++i) {
					for (int l = 0; l < M; ++l) {
						if (l == 0) {
							for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
								for (int t = 0; t < m_grid_div; ++t) {
									if (offspring[P_reserve[i]].Gk()[1] == t + 1 && offspring[P_reserve[i]].Gk()[2] == k + 1)
										m_S[l][m_grid_div*k + t].push_back(P_reserve[i]);
								}
							}
						}
						else if (l == 1) {
							for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
								for (int t = 0; t < m_grid_div; ++t) {
									if (offspring[P_reserve[i]].Gk()[0] == t + 1 && offspring[P_reserve[i]].Gk()[2] == k + 1)
										m_S[l][m_grid_div*k + t].push_back(P_reserve[i]);
								}
							}
						}
						else {
							for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
								for (int t = 0; t < m_grid_div; ++t) {
									if (offspring[P_reserve[i]].Gk()[0] == t + 1 && offspring[P_reserve[i]].Gk()[1] == k + 1)
										m_S[l][m_grid_div*k + t].push_back(P_reserve[i]);
								}
							}
						}
					}
				}
			}
			/*test m_S*/
			for (int l = 0; l < M; ++l) {
				int num = 0;
				for (int k = 0; k < size; ++k) {
					num += m_S[l][k].size();
				}
				if (M == 2 && num != P_reserve.size())
					std::cout << "assign m_S is error:num=" << num << std::endl;
			}
			
		}

		void CDGMOEA_pop::RBS(std::vector<individual> &offspring, std::vector<int> P_reserve) {
			int M = global::ms_global->m_problem->objective_size();
			std::vector<std::vector<int>> R_(P_reserve.size(), std::vector<int>(M));
			/*Decomposition_based Ranking*/
			for (int l = 0; l < M; ++l) {
				for (int k = 0; k < pow(m_grid_div, M - 1); ++k) {
					std::vector<std::pair<double, int>> v;	//first store objective value, second store index
					for (int j = 0; j < m_S[l][k].size(); ++j) {
						v.push_back(std::make_pair(offspring[m_S[l][k][j]].objective()[l], m_S[l][k][j]));
					}
					if (v.size()) {
						sortl(v);	//sort of objective value according to the ascending
						for (int i = 0; i < m_S[l][k].size(); ++i) {
							offspring[v[i].second].R()[l] = i + 1;
						}
					}					
				}
			}
			/*Lexicographic Sorting*/
			for (int i = 0; i < P_reserve.size(); ++i) {
				m_R.push_back(offspring[P_reserve[i]].R());
				sortR(m_R[i], R_[i]);
			}
			std::vector<int> number;	//to get index of P_reserve in offspring
			for (int i = 0; i < P_reserve.size(); ++i) {
				number.push_back(i);
			}
			sortL(R_, number);
			for (int i = 0; i < this->m_inds.size(); ++i) {
				*this->m_inds[i] = offspring[P_reserve[number[i]]];				
			}
			/*free the memory*/			
			std::vector<std::vector<int>>().swap(m_R);
			for (int l = 0; l < M; ++l) {
				for (int k = 0; k < pow(m_grid_div, M - 1); ++k)
					if (m_S[l][k].size() != 0)
						std::vector<int>().swap(m_S[l][k]);	//clear m_S	
			}								

			/*for (int i = 0; i < offspring.size(); ++i) {
				std::vector<int>().swap(offspring[i].GN());
			}*/				

		}

		void CDGMOEA_pop::sortl(std::vector<std::pair<double, int>>& v) {	//selection sort
			for (int i = 0; i < v.size(); ++i) {
				int min = i;
				for (int j = i + 1; j < v.size(); ++j) {
					if (v[j].first < v[min].first)
						min = j;
				}
				auto temp = v[i];
				v[i] = v[min];
				v[min] = temp;
			}
		}

		void CDGMOEA_pop::sortR(const std::vector<int>& R, std::vector<int>& R_) {
			for (int i = 0; i < R.size(); ++i)
				R_[i] = R[i];
			for (int i = 0; i < R_.size(); ++i) {
				int min = i;
				for (int j = i + 1; j < R_.size(); ++j) {
					if (R_[j] < R_[min])
						min = j;
				}
				auto temp = R_[i];
				R_[i] = R_[min];
				R_[min] = temp;
			}
		}

		void CDGMOEA_pop::sortL(std::vector<std::vector<int>>& R_, std::vector<int>& number) {
			int M = global::ms_global->m_problem->objective_size();
			for (int l = 0; l < M; ++l) {
				for (int i = 0; i < R_.size(); ++i) {
					int min = i;
					for (int j = i + 1; j < R_.size(); ++j) {
						if (l == 0 && R_[j][l] < R_[min][l])
							min = j;
						else if (l == 1 && R_[j][0] == R_[min][0] && R_[j][l] < R_[min][l])
							min = j;
						else if (l == 2 && R_[j][0] == R_[min][0] && R_[j][1] == R_[min][1] && R_[j][l] < R_[min][l])
							min = j;
					}
					if (i != min) {
						auto temp1 = R_[i];
						R_[i] = R_[min];
						R_[min] = temp1;
						int temp2 = number[i];
						number[i] = number[min];
						number[min] = temp2;
					}
				}
			}
		}

		void CDGMOEA_pop::select_random(int number, std::vector<int>& result) {
			std::vector<int> candidate;
			for (int i = 0; i < m_inds.size(); ++i) {
				candidate.push_back(i);
			}
			result.resize(number);
			for (int i = 0; i < number; ++i) {
				int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
				result[i] = candidate[idx];
				if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
			}
		}

		void CDGMOEA_pop::nondominated_sorting(std::vector<individual> &offspring) {
			std::vector<std::vector<objective_type>*> objs;
			for (auto& i : offspring)
				objs.emplace_back(&i.objective());
			std::vector<int> rank;
			NS::fast_sort<objective_type>(objs, rank, global::ms_global->m_problem->opt_mode());
			for (size_t i = 0; i < offspring.size(); ++i)
				offspring[i].set_rank(rank[i]);
		}	

		/*void CDGMOEA_pop::record_x() {
			int n = global::ms_global->m_problem->variable_size();
			if (this->m_iter == 1) {
				std::ofstream out;
				out.open("./result/CDG_MOEA_var.txt");
				if (out && n == 2) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << this->m_inds[i]->variable()[j];
						}
						out << std::endl;
					}
				}
				else if (out && n == 3) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::setw(10) << "x3" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << this->m_inds[i]->variable()[j];
						}
						out << std::endl;
					}
				}
				out.close();
			}
			else {
				std::ofstream out;
				out.open("./result/CDG_MOEA_var.txt", std::ios::app);
				if (out && n == 2) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << this->m_inds[i]->variable()[j];
						}
						out << std::endl;
					}
				}
				else if (out && n == 3) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::setw(10) << "x3" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << this->m_inds[i]->variable()[j];
						}
						out << std::endl;
					}
				}
				out.close();
			}

		}

		void CDGMOEA_pop::record_x_offspring() {
			int n = global::ms_global->m_problem->variable_size();
			if (this->m_iter == 1) {
				std::ofstream out;
				out.open("./result/CDG_MOEA_var_offspring.txt");
				if (out && n == 2) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
					for (int i = 0; i < this->m_offspring.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << m_offspring[i].variable()[j];
						}
						out << std::endl;
					}
				}			
				out.close();
			}
			else {
				std::ofstream out;
				out.open("./result/CDG_MOEA_var_offspring.txt", std::ios::app);
				if (out && n == 2) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
					for (int i = 0; i < this->m_offspring.size(); ++i) {
						for (int j = 0; j < n; ++j) {
							out << std::setw(10) << m_offspring[i].variable()[j];
						}
						out << std::endl;
					}
				}				
				out.close();
			}

		}
		
		void CDGMOEA_pop::record_f() {
			int m = global::ms_global->m_problem->objective_size();
			if (this->m_iter == 1) {
				std::ofstream out;
				out.open("./result/CDG_MOEA_obj.txt");
				if (out && m == 2) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << "f1\t" << "f2" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << "\t" << this->m_inds[i]->objective()[j];
						}
						out << std::endl;
					}
				}
				else if (out && m == 3) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::setw(10) << "f3" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << std::setw(10) << this->m_inds[i]->objective()[j];
						}
						out << std::endl;
					}
				}
				out.close();
			}
			else {				
				std::ofstream out;
				out.open("./result/CDG_MOEA_obj.txt", std::ios::app);
				if (out && m == 2) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << std::setw(10) << this->m_inds[i]->objective()[j];
						}
						out << std::endl;
					}
				}
				else if (out && m == 3) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::setw(10) << "f3" << std::endl;
					for (int i = 0; i < this->m_inds.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << std::setw(10) << this->m_inds[i]->objective()[j];
						}
						out << std::endl;
					}
				}
				out.close();
			}
		}
		
		void CDGMOEA_pop::record_f_offspring() {
			int m = global::ms_global->m_problem->objective_size();
			if (this->m_iter == 1) {
				std::ofstream out;
				out.open("./result/CDG_MOEA_obj_offspring.txt");
				if (out && m == 2) {
					out << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
					for (int i = 0; i < this->m_offspring.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << std::setw(10) << this->m_offspring[i].objective()[j];
						}
						out << std::endl;
					}
				}				
				out.close();
			}
			else {
				std::ofstream out;
				out.open("./result/CDG_MOEA_obj_offspring.txt", std::ios::app);
				if (out && m == 2) {
					out << std::endl << "the  " << this->m_iter << "  generation" << std::endl;
					out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
					for (int i = 0; i < this->m_offspring.size(); ++i) {
						for (int j = 0; j < m; ++j) {
							out << std::setw(10) << this->m_offspring[i].objective()[j];
						}
						out << std::endl;
					}
				}				
				out.close();
			}

		}*/

		evaluation_tag CDGMOEA_pop::evolve() {
			evaluation_tag tag = evaluation_tag::Normal;
			//std::cout << "the " << this->m_iter << " generation" << std::endl;
			/*step2:reproduction*/
			tag = evolve_mo();
			/*step3:update of the ideal and nadir points*/
			update_ideal_point(m_offspring);
			if ((this->m_iter % 10) == 0 && this->m_iter != 0)
				update_nadir_point(m_offspring);	//updated nadir point every 50 generations
			/*step4:update of the grid system*/
			eval_dens();
			/*record objective every generation*/			
			//record_x();
			//record_x_offspring();
			//record_f();
			//record_f_offspring();
			this->m_iter++;
			return tag;
		}

		evaluation_tag  CDGMOEA_pop::evolve_mo() {		//step2:reproduction	
			evaluation_tag tag = evaluation_tag::Normal;
			//assign NS of individual in P
			if (this->m_iter) {
				for (size_t i = 0; i < m_inds.size(); ++i) {
					std::vector<size_t>().swap(m_inds[i]->GN());
				}
				assign_GN();
			}
			int m = 0;
			for (int i = 0; i < this->m_inds.size(); ++i) {								
				double rand = global::ms_global->m_uniform[caller::Algorithm]->next();	//get a ramdom value in (0,1)				
				std::vector<size_t> result(3);
				if (rand < m_delta && this->m_inds[i]->GN().size()>3) 
					this->select_in_neighborhood(3, this->m_inds[i]->GN(), result);				
				else					
					this->select(i, 3, result);
				/*adopt DE operator in MOEA/D*/				
				m_offspring[m++] = *this->m_inds[i];
				cross_mutate(result, m_offspring[m]);
				tag = m_offspring[m].evaluate();
				if (tag != evaluation_tag::Normal) break;
				m++;
			}
			return tag;			
		}

		void CDGMOEA_pop::eval_dens() {	//step5:rank_based selection
			//delete individual of objective>m_nadir	
			std::vector<int> P_delete, P_reserve;
			for (int i = 0; i < m_offspring.size(); ++i) {
				int flag = 0;
				for (int j = 0; j < global::ms_global->m_problem->objective_size(); ++j) {
					if (this->m_nadir[j] < m_offspring[i].objective()[j])
						flag = 1;					
				}
				if (flag)
					P_delete.push_back(i);
				else
					P_reserve.push_back(i);
			}
			if ((P_delete.size() + P_reserve.size()) != m_offspring.size())
				std::cout << "step3 is error" << std::endl;

			if (P_reserve.size() != 0)			
				gridConstruct_assignGN_P_reserve(m_offspring, P_reserve, P_reserve.size());
			else
				std::cout <<"the "<<this->m_iter<<" generation : "<< "reserve = 0" << std::endl;

			if (P_reserve.size() < this->m_inds.size()) {	// when Preserve < N
				for (int i = 0; i < P_reserve.size(); ++i) {
					*this->m_inds[i] = m_offspring[P_reserve[i]];					
				}
				//select (N-P_reserve) individual from P_delete randomly
				std::vector<int> result(this->m_inds.size() - P_reserve.size());
				this->select_random(this->m_inds.size() - P_reserve.size(), result);				
				int m = 0;
				for (int i = P_reserve.size(); i < this->m_inds.size(); ++i) {
					*this->m_inds[i] = m_offspring[P_delete[result[m++]]];
					//this->m_inds[i]->solut() = m_offspring[P_delete[result[m++]]].solut();
				}
			}
			else {	// when Preserve > N , /*step5:rank_based selection*/ RBS
				assign_S(m_offspring, P_reserve);
				RBS(m_offspring, P_reserve);
			}
		}

		/******************************CDGMOEA***********************************/
		CDGMOEA::CDGMOEA(param_map &v):algorithm(v.at("algName")),m_pop(v.at("popSize")){ }

		void CDGMOEA::initialize() {
			m_pop.initialize();
			m_pop.evaluate();
			m_pop.set_parameter(1.0, 0.5); //set parmeter: CR=1.0, F=0.5
			m_pop.update_ideal_point();
			m_pop.update_nadir_point();
			m_pop.gridConstruct();
			m_pop.assign_GN();
		}

		void CDGMOEA::record() {
			size_t evals = CONTINUOUS_CAST->evaluations();
			real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
			measure::get_measure()->record(global::ms_global.get(), evals, IGD);
		}

		void CDGMOEA::run_() {
			while (!terminating()) {
				m_pop.evolve();
			}
		}

	}
}
