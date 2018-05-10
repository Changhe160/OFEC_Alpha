#include "GL_Cont.h"
#include "adaptor_cont.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif
namespace OFEC {
#ifdef OFEC_DEBUG_
	std::vector<std::vector<double>> GL_cont::ms_div;
#endif

	GL_cont::GL_cont(param_map &v) :GL<population<individual<>>>(v), m_tree(nullptr) {
		m_adaptor.reset(new adaptor<individual<>>(m_numDim, m_popsize));
		this->m_term.reset(new term_max_evals(v));
		//this->m_term.reset(new TermMean(200,1.e-5,mean()));
		if (v.find("alpha") != v.end()) m_alpha = v["alpha"];
		else	m_alpha = -1;

		buildtree();
		m_numActive = m_popsize;
	}

	void GL_cont::initilizeCurPop()
	{
		m_curPop.resize(this->m_popsize);
		for (int i = 0; i < this->m_popsize; i++) {
			m_curPop[i].data() = m_pop[i]->data();
		}
	}

	void GL_cont::buildtree() {
		std::vector<pair<double, double>> box(m_numDim);
		for (int i = 0; i < m_numDim; ++i) {
			CAST_PROBLEM_CONT->getSearchRange(box[i].first, box[i].second, i);
		}
		std::vector<double> ratio(m_popsize, 1. / m_popsize);
		m_tree.reset(new KDTreeSpace::PartitioningKDTree<double>(m_numDim, ratio, box));
		m_tree->buildIndex();

	}
	bool GL_cont::removeConverged() {

		map<int, list<int>> group;
		for (int i = 0; i < this->m_popsize; i++) {
			int idx = m_tree->get_regionIdx(m_pop[i]->data().m_x);
			auto j = group[idx].begin();
			while (j != group[idx].end()) {
				if (*m_pop[i] < *m_pop[*j]) ++j;
				else break;
			}
			group[idx].insert(j, i);
		}
		int actgroup = 0, actind = 0;
		double ratio = 0;
		for (auto &i : group) {
			if (i.second.size() > 1 && Global::g_arg.find(param_epsilon) != Global::g_arg.end()) {
				double d = 0, vol = m_tree->getBoxVolume(i.first);
				for (auto j = ++i.second.begin(); j != i.second.end(); ++j) {
					//d += m_pop[i.second.front()]->getDistance(m_pop[*j]->data());
					d += m_pop[i.second.front()]->data().getObjDistance(m_pop[*j]->data().m_obj);
				}
				d /= i.second.size();
				ratio += d / vol;
				if (Global::g_arg[param_epsilon] > d) {// converged at region idx
					for (auto j : i.second) {
						m_pop[j]->setActive(false);
					}
				}
				else {
					for (auto j : i.second) 				m_pop[j]->setActive(true);
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
		for (int i = 0; i < this->m_popsize; i++) if (m_pop[i]->isActive()) m_numActive++;
		//cout << group.size() << " "<<actgroup<<" "<< actind<<" "<< ratio<<" ";

		return false;
	}

	ReturnFlag GL_cont::evolve() {

		dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->updateStep(m_pop);

		if (m_ms != UpdateScheme::hb) {
			m_adaptor->createSolutions(m_pop, m_curPop, m_alpha);
		}
		else {
			m_adaptor->createSolutions(m_pop, m_curPop, m_hisIndi, m_alpha);
		}

		ReturnFlag rf = updata();

		for (auto &i : m_pop) {
			if (i->getFlag()) updateBestArchive(i->data());
		}

		updateMemory();
		m_iter++;

		return rf;
	}

	void GL_cont::updateBeta(std::vector<double>&beta) {
		int maxsel = 0, idx = -1;
		for (auto i = 0; i < m_accpb.size(); ++i) {
			//m_hisprog[i] = (m_hisprog[i] + m_prog[i].first) / 2;
			//if (i == 0)			m_accprog[i] = m_hisprog[i];
			//else m_accprog[i] = m_accprog[i-1]+ m_hisprog[i];

			if (i == 0)			m_accpb[i] = m_pb[i].first;
			else m_accpb[i] = m_accpb[i - 1] + m_pb[i].first;
			if (maxsel < m_pb[i].second) {
				maxsel = m_pb[i].second;
				idx = i;
			}
			m_pb[i].second = 0;
			m_pb[i].first = 0;
		}
		if (idx == beta.size() - 1 && beta[idx] < 40) beta[idx] += 0.5;
	}

	void GL_cont::updateAlpha(std::vector<double>&alpha) {
		int maxsel = 0, idx = -1;
		for (auto i = 0; i < m_accpa.size(); ++i) {
			//m_hisprog[i] = (m_hisprog[i] + m_prog[i].first) / 2;
			//if (i == 0)			m_accprog[i] = m_hisprog[i];
			//else m_accprog[i] = m_accprog[i-1]+ m_hisprog[i];

			if (i == 0)			m_accpa[i] = m_pa[i].first;
			else m_accpa[i] = m_accpa[i - 1] + m_pa[i].first;
			if (maxsel < m_pa[i].second) {
				maxsel = m_pa[i].second;
				idx = i;
			}
			m_pa[i].second = 0;
			m_pa[i].first = 0;
		}
		if (idx == 0 && alpha[idx] > 0.1) alpha[idx] -= 0.001;
		else if (idx == alpha.size() - 1 && alpha[idx] < 0.999) alpha[idx] += 0.001;
	}

	void GL_cont::initializepar(std::vector<double>&beta, std::vector<double>&alpha) {
		m_pb.resize(beta.size(), pair<double, int>(0, 0));
		m_accpb.resize(beta.size(), 0);
		m_hispb.resize(beta.size(), 0);

		m_pa.resize(alpha.size(), pair<double, int>(0, 0));
		m_accpa.resize(alpha.size(), 0);
		m_hispa.resize(alpha.size(), 0);
	}
	ReturnFlag GL_cont::run_() {
		ReturnFlag rf = Return_Normal;

#ifdef OFEC_DEBUG_
		g_mutex.lock();
		if (GL_cont::ms_div.size() == 0) GL_cont::ms_div.resize(Global::g_arg[param_numRun]);
		g_mutex.unlock();
#endif
		rf = initialize(false, true, true);
		//if (Global::g_arg.find(param_beta) == Global::g_arg.end()) m_beta=0;
		//if (Global::g_arg.find(param_xoverProbability) == Global::g_arg.end()) m_alpha=0.95;

		initilizeMemory();
		initilizeCurPop();

		std::vector<double> beta = { 0,1,3,5,10,11 };
		std::vector<double>alpha = { 0.7,0.8,0.9,0.95 };
		initializepar(beta, alpha);

		double cmean, pmean;
		bool randbeta = false, randalpha = false;
		//if (m_alpha<0) randalpha = true;
		//if (m_beta<0) randbeta = true;
		double initial_beta = m_beta;

		cmean = pmean = m_best[0]->data().m_obj[0];
		int selbeta = 0, selalpha;

		std::vector<Solution<CodeVReal>*> P;
		for (int i = 0; i < m_pop.size(); ++i) {
			P.push_back(m_pop[i].get());
		}
		while (rf != Return_Terminate)
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

			if (randbeta&&randalpha&&m_iter % (beta.size() * 5) == 0) {
				updateBeta(beta);
				updateAlpha(alpha);

			}
			dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->updateStep(m_pop);


			pmean = cmean;

			if (randalpha) {
				if (m_accpa.back() > 0) {
					double p = Global::msp_global->mp_uniformAlg->Next()*m_accpa.back();
					selalpha = lower_bound(m_accpa.begin(), m_accpa.end(), p) - m_accpa.begin();
				}
				else {
					selalpha = Global::msp_global->getRandInt(0, alpha.size());
				}

				m_pa[selalpha].second++;
				m_alpha = alpha[selalpha];
			}

			if (m_ms != UpdateScheme::hb) {
				m_adaptor->createSolutions(m_pop, m_curPop, m_alpha);
				//if (Global::msp_global->mp_problem->isProTag(MMP))		m_adaptor->createSolutions(m_pop, m_curPop, m_alpha);
				//else if(m_alpha<0)		dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->createSolutions(m_pop, m_curPop, xp);
				//else m_adaptor->createSolutions(m_pop, m_curPop, m_alpha);
			}
			else {
				m_adaptor->createSolutions(m_pop, m_curPop, m_hisIndi, m_alpha);
			}
			rf = updata();

			for (auto &i : m_pop) {
				if (i->getFlag()) {
					updateBestArchive(i->data());
					if (randalpha)		m_pa[selalpha].first += 1;
				}

			}
			cmean = m_best[0]->data().m_obj[0];
			//cout << pmean << " " << cmean << endl;
			if (randbeta)		m_pb[selbeta].first += (cmean == pmean ? 0 : 1);
			//m_pa[selalpha].first += (cmean==pmean?0:1);

#ifdef OFEC_CONSOLE
			if (mMultiModal::getPopInfor()) {
				int peaksf;
				peaksf = CAST_PROBLEM_CONT->getGOpt().getNumGOptFound();
				mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(), \
					Global::msp_global->m_totalNumIndis, 1, peaksf, \
					CAST_PROBLEM_CONT->getGOpt().getNumOpt(), 0, 0, 0, 0, \
					0, 0, CAST_PROBLEM_CONT->getGOpt().isAllFound());
			}
#endif

			/*std::vector<double> gopt(1);
			Global::msp_global->mp_problem->getObjGlobalOpt(gopt);
			cout << Global::g_arg[param_proName] << " " << m_iter << " " << setw(15) << setprecision(15) << fabs(Solution<CodeVReal>::getBestSolutionSoFar().data().m_obj[0] - gopt[0]) << " ";
			cout << dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->getIntraDis(m_pop) << " ";
			cout << CAST_PROBLEM_CONT->getGOpt().getNumGOptFound() << " " << CAST_PROBLEM_CONT->getGOpt().distance(P) << " " << endl;*/

			//cout << variance(mean()) << endl;
			//m_adaptor->printPro();
			/*if (dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->getInterDis() < 1.e-5) {
				rf = Return_Terminate;
				mSingleObj::getSingleObj()->setLastEvaluations(Global::msp_global.get());
			}*/

			if (rf == Return_Terminate) break;
			/*if (updateVisitation() == 0) {
				clustering();
				plotHistory();
				rf = redistribute();
			}*/
			//if (Global::msp_global->mp_problem->isProTag(MMP)) 	removeConverged();


			rf = dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->repel(m_pop);

			if (rf == Return_Terminate) break;

			if (initial_beta < 0) m_beta = Global::msp_global->getRandFloat(0, 20);

			if (m_numActive == 0) break;
			if (randbeta) {
				if (m_accpb.back() > 0) {
					double p = Global::msp_global->mp_uniformAlg->Next()*m_accpb.back();
					selbeta = lower_bound(m_accpb.begin(), m_accpb.end(), p) - m_accpb.begin();
				}
				else selbeta = Global::msp_global->getRandInt(0, beta.size());
				m_beta = beta[selbeta];
				m_pb[selbeta].second++;
				updateMemory(true);
			}
			else
				updateMemory();


			resetFlag();

			m_iter++;


		}
#ifdef OFEC_CONSOLE
		g_mutex.lock();
		static double inter_dis = 0, intra_dis = 0;
		string ss = Global::g_arg[param_workingDir];
		ss += "Result/";
		ss += mSingleObj::getSingleObj()->m_fileName.str();
		ss += "conv.txt";
		inter_dis += dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->getInterDis();
		intra_dis += dynamic_cast<MDLOperator_Cont<Individual<CodeVReal>>*>(m_adaptor.get())->getIntraDis(m_pop);
		ofstream out(ss.c_str());
		out << inter_dis / (int)MAX_NUM_RUN << endl;
		out.close();
		g_mutex.unlock();

		//cout << CAST_PROBLEM_CONT->getGOpt().getNumGOptFound() << endl;
#endif
		return rf;
	}


#ifdef OFEC_DEBUG_
	void  GL_cont::output() {
		stringstream ss;
		ss << Global::g_arg[param_workingDir] << "Result/" << mSingleObj::getSingleObj()->getFileName() << "div.txt";
		ofstream out(ss.str());
		int i = 0;
		while (1) {
			double sum = 0;
			int cnt = 0;
			for (auto &j : ms_div) {
				if (i < j.size()) {
					sum += j[i];
					cnt++;
				}
			}
			if (cnt == 0) break;
			out << i*Global::g_arg[param_popSize] << " " << sum / cnt << endl;
			++i;
		}
		out.close();
	}
#endif
}