#include "MDLACO.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#include "MDLOperator_TSP.h"
#include "MDLOperator_QAP.h"
#include "MDLOperator_MKP.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

static mutex m_nearMutex;
vector<set<int>> MDLACO::mv_candidate;

MDLACO::MDLACO(ParamMap &v):MDL<CodeVInt, Ant, Population<CodeVInt, Ant>>(v), m_num(0){

	if (Global::msp_global->mp_problem->isProTag(TSP)) {
		m_MDLOperator.reset(new MDLOperator_TSP<Ant>(m_numDim));
	}
	else if (Global::msp_global->mp_problem->isProTag(QAP)) {
		m_MDLOperator.reset(new MDLOperator_QAP<Ant>(m_numDim));
	}
	else if (Global::msp_global->mp_problem->isProTag(MKP)) {
		m_MDLOperator.reset(new MDLOperator_MKP<Ant>(m_numDim));
	}
}
void MDLACO::resetAntsInfo()
{
	for (int i = 0; i<m_popsize; i++)
		m_curPop[i].resetData();
}


double MDLACO::getLenOfNN()
{
	vector<int> candidate(m_numDim), result(m_numDim);
	TravellingSalesman *_ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
	const vector<vector<double>> cost = _ptr->getCost();
	int n = 0;
	for (int i = 0; i < candidate.size(); i++) {
		candidate[i] = i;
	}
	result[n++] = candidate[0];
	candidate[0] = candidate[m_numDim - 1];
	while (n < m_numDim) {
		int loc = 0;
		double min = cost[result[n - 1]][candidate[loc]];
		for (int m = 1; m < m_numDim - n; m++) {
			if (cost[result[n - 1]][candidate[m]] < min) {
				min = cost[result[n - 1]][candidate[m]];
				loc = m;
			}
		}
		result[n++] = candidate[loc];
		candidate[loc] = candidate[m_numDim - n];
	}
	double val = 0;
	for (int i = 0; i < m_numDim; i++) {
		val += cost[result[i]][result[(i + 1) % m_numDim]];
	}
	return val;
}

ReturnFlag MDLACO::initialize_AS()
{
	ReturnFlag rf;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	int dim = m_pop[0]->getNumDim();
	vector<vector<double>> phero;
	phero.resize(dim);
	for (int i = 0; i < dim; i++)
		phero[i].resize(dim);
	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++)
			phero[i][j] = 1. / dim;
	for (int i = 1; i<dim; i++)
		for (int j = 0; j<m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, 5, 1);
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
	return rf;
}


ReturnFlag MDLACO::initialize_ACS()
{
	ReturnFlag rf;
	double mt = getLenOfNN();
	mt = 1.0 / (m_numDim * mt);
	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i<m_numDim; i++)
		for (int j = 0; j<m_numDim; j++)
			phero[i][j] = 1. / m_numDim;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 0; i < m_popsize; i++)
	{
		for (int j = 1; j < m_numDim; j++)
		{
			double q = Global::msp_global->mp_uniformAlg->Next();
			if (q <= 0.9)
				m_pop[i]->selectNextCity_Greedy(phero, 2);
			else
				m_pop[i]->selectNextCity_Pro(phero, 2);
			local_updatePheromeno(phero, mt, i);
		}
		local_updatePheromeno(phero, mt, i, true);
	}
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
	return rf;
}


ReturnFlag MDLACO::initialize_MMAS()
{
	ReturnFlag rf;
	int length = 20;
	if (m_numDim <= 20)
		length = m_numDim - 1;
	double pheroMax = getLenOfNN();
	pheroMax = 1. / (0.02 * pheroMax);

	m_nearMutex.lock();

	if (mv_candidate.empty())
	{
		vector<vector<int>> temp;
		mv_candidate.resize(m_numDim);
		temp.resize(m_numDim);
		for (int i = 0; i < temp.size(); i++)
			temp[i].resize(length);
		dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get())->findNearbyCity(temp);
		for (int i = 0; i < temp.size(); i++)
		{
			for (int j = 0; j < length; j++)
			{
				mv_candidate[i].insert(temp[i][j]);
			}
		}
	}

	m_nearMutex.unlock();

	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		for (int j = 0; j < m_numDim; j++)
			phero[i][j] = pheroMax;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 1; i < m_numDim; i++)
		for (int j = 0; j < m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, mv_candidate, 2, 1);

	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
	return rf;
}


ReturnFlag MDLACO::run_()

{
	ReturnFlag rf = Return_Normal;
	int mode = Global::g_arg[param_populationInitialMethod];
	PopInitMethod popInitialMode = static_cast<PopInitMethod>(mode);
	if (popInitialMode == POP_INIT_AS)
		rf = initialize_AS();
	else if (popInitialMode == POP_INIT_ACS)
		rf = initialize_ACS();
	else if (popInitialMode == POP_INIT_MMAS)
		rf = initialize_MMAS();
	else {
		rf = initialize(false, true, true);
		//statisticDiffsAndImp();
#ifdef OFEC_CONSOLE
		if (Global::msp_global->mp_problem->isProTag(TSP))
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize);
#endif
	}

	initilizeMemory();
	initilizeCurPop();

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());
	
	while (rf!=Return_Terminate)
	{
#ifdef OFEC_DEMON
		for (int i = 0; i<this->size(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		
		m_MDLOperator->createSolutions(m_pop, m_curPop, m_alpha);

		rf = updata();

		//statisticDiffsAndImp();
#ifdef OFEC_CONSOLE
		if (Global::msp_global->mp_problem->isProTag(TSP))
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize);
#endif
		
		m_iter++;
		dynamic_cast<TermMean*>(m_term.get())->countSucIter(mean());
		updateMemory();
		resetFlag();
		resetAntsInfo();
		vector<double> gopt(1);
		Global::msp_global->mp_problem->getObjGlobalOpt(gopt);
		cout << setw(10)<<m_iter <<" "<< setw(10) << mean() << setw(15) << Solution<CodeVInt>::getBestSolutionSoFar().data().m_obj[0]- gopt[0] <<" ";
		m_MDLOperator->printPro();
		if (rf == Return_Terminate|| m_term->ifTerminating()) break;
	}
	//dynamic_cast<MDLOperator_TSP<Ant>*>(m_MDLOperator.get())->printMatrix(m_iter);

	mSingleObj::getSingleObj()->setLastEvaluations(Global::msp_global.get());
#ifdef OFEC_CONSOLE
	if (Global::msp_global->mp_problem->isProTag(TSP)) {
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, false);
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
	}
#endif
	return Return_Terminate;
}

void MDLACO::local_updatePheromeno(vector<vector<double>> &phero, double t, int ant_loc, bool isLastEdge)
{
	pair<int, int> edge;
	if (!isLastEdge)
		edge = m_pop[ant_loc]->getCurrentEdge();
	else
		edge = m_pop[ant_loc]->getLastEdge();
	if (edge.first == edge.second && edge.first == -1)
		throw myException("edge error in local_updatePheromeno function @ACS.cpp");
	phero[edge.first][edge.second] = (1 - 0.1)*phero[edge.first][edge.second] + 0.1*t;
	phero[edge.second][edge.first] = (1 - 0.1)*phero[edge.second][edge.first] + 0.1*t;  //symmetric
}


void MDLACO::statisticDiffsAndImp()
{
#ifdef OFEC_CONSOLE
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}