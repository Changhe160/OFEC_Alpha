#include "NSGAII.h"
#include "../../../../utility/nondominated_sorting/fast_sort/fast_sort.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <algorithm>

namespace OFEC {
	NSGAII::NSGAII(param_map & v) : population((int)(v.at("popSize")), global::ms_global->m_problem->variable_size()), m_offspring(2 * (int)(v.at("popSize"))), \
		m_cr(1.0), m_ceta(30), m_meta(20), m_mr(1.0 / (double)(global::ms_global->m_problem->variable_size())) {
		set_name(v.at("algName"));
		set_termination(new term_max_evals(v));
		for (auto& i : m_offspring)
			i = std::move(std::shared_ptr<individual<>>(new individual<>(global::ms_global->m_problem->objective_size(), global::ms_global->m_problem->variable_size())));
	}
	NSGAII::NSGAII(const std::string& name, int size_pop, int max_evals) : population(size_pop, global::ms_global->m_problem->variable_size()), m_offspring(2 * size_pop), \
		m_cr(1.0), m_ceta(30), m_meta(20), m_mr(1.0 / (double)(global::ms_global->m_problem->variable_size())) {
		set_name(name);
		set_termination(new term_max_evals(max_evals));
		for (auto& i : m_offspring)
			i = std::move(std::shared_ptr<individual<>>(new individual<>(global::ms_global->m_problem->objective_size(), global::ms_global->m_problem->variable_size())));
	}
	evaluation_tag NSGAII::run_() {

		int evals = global::ms_global->m_problem->total_evaluations();
		double IGD = CONTINOUS_CAST->get_optima().IGD_to_PF(*this);
		measure::ms_measure->record(global::ms_global.get(), evals, IGD);

		// evolution
		while (!terminating())
		{
			evolve_mo();
			sort();
			eval_dens();

			evals = global::ms_global->m_problem->total_evaluations();
			if (evals % (int)global::ms_arg.at("sampleFre") == 0) {
				IGD = CONTINOUS_CAST->get_optima().IGD_to_PF(*this);
				measure::ms_measure->record(global::ms_global.get(), evals, IGD);
			}
		}
		return evaluation_tag::Normal;
	}
	void NSGAII::sort() {
		size_t pop_size = m_offspring.size();
		std::vector<std::vector<double>> pop_obj(pop_size);
		for (size_t i = 0; i < pop_size; i++) {
			pop_obj[i] = m_offspring[i]->get_objective();
		}
		std::vector<int> rank;
		std::pair<int, int> meas(0, 0);

		NS::fast_sort(pop_obj, rank, meas);

		for (size_t i = 0; i < pop_size; i++) {
			m_offspring[i]->set_rank(rank[i]);
		}
	}
	void NSGAII::eval_dens() {
		int numobj = global::ms_global->m_problem->objective_size();
		int pops = 0;  //indicate parent population size be 0
		int size = m_offspring.size();
		int rank = 0;
		while (1) {
			int count = 0;
			for (size_t i = 0; i<size; i++)
				if (m_offspring[i]->rank() == rank)
					count++;

			int size2 = pops + count;
			if (size2 > m_pop.size()) {
				break;
			}

			for (size_t i = 0; i<size; i++)
				if (m_offspring[i]->rank() == rank)
				{
					*m_pop[pops] = *m_offspring[i];
					++pops;
				}

			rank++;
			if (pops >= m_pop.size()) break;
		}

		if (pops<m_pop.size()) {
			std::vector<int> list;
			// save the individuals in the overflowed front
			for (size_t i = 0; i<size; i++)
				if (m_offspring[i]->rank() == rank)
					list.push_back(i);
			int s2 = list.size();
			std::vector<double> density(s2);
			std::vector<double> obj(s2);
			std::vector<int> idx(s2);
			std::vector<int> idd(s2);

			for (size_t i = 0; i<s2; i++) {
				idx[i] = i;
				density[i] = 0;
			}

			for (size_t j = 0; j<numobj; j++) {
				for (size_t i = 0; i<s2; i++) {
					idd[i] = i;
					obj[i] = m_offspring[list[i]]->get_objective()[j];
				}
				//gMinfastsort(obj,idd,s2,s2);
				quick_sort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k<s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			idd.clear();
			obj.clear();

			int s3 = m_pop.size() - pops;

			//gMinfastsort(density,idx,s2,s3);
			quick_sort(density, s2, idx, true, 0, s2 - 1, s3);
			for (size_t i = 0; i<s3; i++)
			{
				*m_pop[pops] = *m_offspring[list[idx[i]]];
				++pops;
			}

			density.clear();
			idx.clear();
			list.clear();
		}
	}
	void NSGAII::evolve_mo() {
		int m = 0;
		if (m_pop.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAII::evolve_mo()");
		for (int n = 0; n<m_pop.size(); n += 2) {
			std::vector<int> p(2);
			p[0] = tour_selection();
			while (1) { p[1] = tour_selection();  	if (p[1] != p[0]) break; }
			cross_mutate(p, m_offspring[m].get(), m_offspring[m + 1].get());
			m += 2;
			*m_offspring[m++] = *m_pop[n];
			*m_offspring[m++] = *m_pop[n + 1];
		}
	}
	int NSGAII::tour_selection() {
		int p1 = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, m_pop.size());
		int p2 = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, m_pop.size());

		if (m_pop[p1]->rank() < m_pop[p2]->rank())
			return p1;
		else
			return p2;
	}
	double NSGAII::get_betaq(double rand, double alpha, double ceta) {
		double betaq = 0.0;
		if (rand <= (1.0 / alpha))
		{
			betaq = pow((rand*alpha), (1.0 / (ceta + 1.0)));
		}
		else
		{
			betaq = pow((1.0 / (2.0 - rand * alpha)), (1.0 / (ceta + 1.0)));
		}
		return betaq;
	}
	void NSGAII::PolynomialMutation(individual<>* indv, double mr, double meta) {
		auto &x = indv->get_variable();

		for (size_t i = 0; i<x.size(); i += 1)
		{
			if (global::ms_global->m_uniform[caller::Algorithm]->next() <= mr)
			{

				double y = x[i],
					lb = CONTINOUS_CAST->range(i).first,
					ub = CONTINOUS_CAST->range(i).second;

				double delta1 = (y - lb) / (ub - lb),
					delta2 = (ub - y) / (ub - lb);

				double mut_pow = 1.0 / (meta + 1.0);

				double rnd = global::ms_global->m_uniform[caller::Algorithm]->next(), deltaq = 0.0;
				if (rnd <= 0.5)
				{
					double xy = 1.0 - delta1;
					double val = 2.0*rnd + (1.0 - 2.0*rnd)*(pow(xy, (meta + 1.0)));
					deltaq = pow(val, mut_pow) - 1.0;
				}
				else
				{
					double xy = 1.0 - delta2;
					double val = 2.0*(1.0 - rnd) + 2.0*(rnd - 0.5)*(pow(xy, (meta + 1.0)));
					deltaq = 1.0 - (pow(val, mut_pow));
				}

				y = y + deltaq * (ub - lb);
				y = std::min(ub, std::max(lb, y));

				x[i] = y;
			}
		}
	}
	void NSGAII::SimulatedBinaryCrossover(individual<>* child1, individual<>* child2, const individual<>& parent1, const individual<>& parent2, double cr, double ceta) {
		child1->get_variable() = parent1.get_variable();
		child2->get_variable() = parent2.get_variable();

		if (global::ms_global->m_uniform[caller::Algorithm]->next() > cr) return; // not crossovered

		auto &c1 = child1->get_variable(), &c2 = child2->get_variable();
		const auto &p1 = parent1.get_variable(), &p2 = parent2.get_variable();

		for (size_t i = 0; i<c1.size(); i += 1)
		{
			if (global::ms_global->m_uniform[caller::Algorithm]->next() > 0.5) continue; // these two variables are not crossovered
			if (std::fabs(static_cast<double>(p1[i]) - static_cast<double>(p2[i])) <= 1.0e-14) continue; // two values are the same

			double y1 = std::min(p1[i], p2[i]),
				y2 = std::max(p1[i], p2[i]);

			double lb = CONTINOUS_CAST->range(i).first,
				ub = CONTINOUS_CAST->range(i).second;

			double rand = global::ms_global->m_uniform[caller::Algorithm]->next();

			// child 1
			double beta = 1.0 + (2.0*(y1 - lb) / (y2 - y1)),
				alpha = 2.0 - pow(beta, -(ceta + 1.0));
			double betaq = get_betaq(rand, alpha, ceta);

			c1[i] = 0.5*((y1 + y2) - betaq * (y2 - y1));

			// child 2
			beta = 1.0 + (2.0*(ub - y2) / (y2 - y1));
			alpha = 2.0 - pow(beta, -(ceta + 1.0));
			betaq = get_betaq(rand, alpha, ceta);

			c2[i] = 0.5*((y1 + y2) + betaq * (y2 - y1));

			// boundary checking
			c1[i] = std::min(ub, std::max(lb, static_cast<double>(c1[i])));
			c2[i] = std::min(ub, std::max(lb, static_cast<double>(c2[i])));

			if (global::ms_global->m_uniform[caller::Algorithm]->next() <= 0.5)
			{
				std::swap(c1[i], c2[i]);
			}
		}
	}
	void NSGAII::cross_mutate(const std::vector<int>& index, individual<>* child1, individual<>* child2) {
		SimulatedBinaryCrossover(child1, child2, *population<individual<>>::m_pop[index[0]], *population<individual<>>::m_pop[index[1]], m_cr, m_ceta);

		PolynomialMutation(child1, m_mr, m_meta);
		PolynomialMutation(child2, m_mr, m_meta);

		child1->evaluate();
		child2->evaluate();
	}
}
