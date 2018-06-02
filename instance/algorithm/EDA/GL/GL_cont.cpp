#include "GL_Cont.h"
#include "adaptor_cont.h"
#include <list>
#include "../../../../core/problem/continuous/continuous.h"

#include <iomanip>

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif
namespace OFEC {
	namespace EDA {
#ifdef OFEC_DEBUG_
		std::vector<std::vector<double>> GL_cont::ms_div;
#endif

		GL_cont::GL_cont(param_map &v) :GL<population<individual<>>>(v), m_tree(nullptr) {
			m_adaptor.reset(new adaptor_cont<individual<>>(global::ms_global->m_problem->variable_size(), this->size()));
			this->m_term.reset(new term_max_evals(v));
			
			if (v.find("alpha") != v.end()) m_alpha = v["alpha"];
			else	m_alpha = -1;

			buildtree();
			m_numActive = this->size();
		}

		void GL_cont::initilize_curpop()
		{
			for (int i = 0; i < this->size(); i++) {
				m_curPop.push_back(*m_pop[i]);
			}
		}

		void GL_cont::buildtree() {
			int numDim = global::ms_global->m_problem->variable_size();
			std::vector<std::pair<double, double>> box(numDim);
			for (int i = 0; i < numDim; ++i) {
				box[i] = CONTINOUS_CAST->range(i);
			}
			std::vector<double> ratio(size(), 1. / size());
			m_tree.reset(new KDTreeSpace::PartitioningKDTree<double>(numDim, ratio, box));
			m_tree->buildIndex();

		}
		bool GL_cont::removeConverged() {

			std::map<int, std::list<int>> group;
			for (int i = 0; i < this->size(); i++) {
				int idx = m_tree->get_regionIdx(m_pop[i]->variable().vect());
				auto j = group[idx].begin();
				while (j != group[idx].end()) {
					if (m_pop[*j]->dominate(*m_pop[i])) ++j;
					else break;
				}
				group[idx].insert(j, i);
			}
			int actgroup = 0, actind = 0;
			double ratio = 0;
			for (auto &i : group) {
				if (i.second.size() > 1 && global::ms_arg.find("epsilon") != global::ms_arg.end()) {
					double d = 0, vol = m_tree->getBoxVolume(i.first);
					for (auto j = ++i.second.begin(); j != i.second.end(); ++j) {
						//d += m_pop[i.second.front()]->getDistance(m_pop[*j]->data());
						d += m_pop[i.second.front()]->objective_distance(*m_pop[*j]);
					}
					d /= i.second.size();
					ratio += d / vol;
					if (global::ms_arg["epsilon"] > d) {// converged at region idx
						for (auto j : i.second) {
							m_pop[j]->set_active_flag(false);
						}
					}
					else {
						for (auto j : i.second) 				m_pop[j]->set_active_flag(true);
						actgroup++;
						actind += i.second.size();
					}
				}
				else {
					actgroup++;
					actind += i.second.size();
				}
			}
			m_numActive = 0;
			for (int i = 0; i < this->size(); i++) if (m_pop[i]->is_active()) m_numActive++;
			//cout << group.size() << " "<<actgroup<<" "<< actind<<" "<< ratio<<" ";

			return false;
		}

		evaluation_tag GL_cont::evolve() {

			dynamic_cast<adaptor_cont<individual<>>*>(m_adaptor.get())->update_step(m_pop);

			if (m_ms != UpdateScheme::hb) {
				m_adaptor->create_solution(m_pop, m_curPop, m_alpha);
			}
			else {
				m_adaptor->create_solution(m_pop, m_curPop, m_hisIndi, m_alpha);
			}

			evaluation_tag rf = update();

			for (auto &i : m_pop) {
				if (i->is_improved()) update_best(*i);
			}

			update_memory();
			m_iter++;

			return rf;
		}

		evaluation_tag GL_cont::run_() {
			evaluation_tag rf = evaluation_tag::Normal;

			rf = initialize();

			initilize_memory();
			initilize_curpop();
			
			while (rf != evaluation_tag::Terminate)
			{
#ifdef OFEC_DEMON
				for (int i = 0; i < this->size(); i++)
					updateBestArchive(this->m_pop[i]->self());
				std::vector<Algorithm*> vp;
				vp.push_back(this);
				msp_buffer->updateBuffer_(&vp);
#endif
#ifdef OFEC_DEBUG_
				GL_cont::ms_div[Global::msp_global->m_runId].push_back(dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->getAvgDis());
#endif


				dynamic_cast<adaptor_cont<individual<>>*>(m_adaptor.get())->update_step(m_pop);

				if (m_ms != UpdateScheme::hb) {
					m_adaptor->create_solution(m_pop, m_curPop, m_alpha);
				}
				else {
					m_adaptor->create_solution(m_pop, m_curPop, m_hisIndi, m_alpha);
				}
				rf = update();

				for (auto &i : m_pop) {
					if (i->is_improved()) {
						update_best(*i);
					}

				}

#ifdef OFEC_CONSOLE
				record();
#endif
				
				std::cout << global::ms_global->m_problem->name() << " " << m_iter << " " << std::setw(15) << std::setprecision(15) << 
					m_best[0]->objective_distance(CONTINOUS_CAST->get_optima().objective() )<< " "<<'\n';

				if (rf == evaluation_tag::Terminate)
					break;
				update_memory();
				reset_improved_flag();
				m_iter++;
			}

			return rf;
		}

		void GL_cont::record() {
			if (measure::get_measure()) {
				if (global::ms_global->m_problem->has_tag(problem_tag::MMOP))
					measure::get_measure()->record(global::ms_global.get(), global::ms_global->m_problem->evaluations(), CONTINOUS_CAST->num_optima_found());
				else
					measure::get_measure()->record(global::ms_global.get(), global::ms_global->m_problem->evaluations(), m_best[0]->objective_distance(CONTINOUS_CAST->get_optima().objective()));

			}
		}
	}
}