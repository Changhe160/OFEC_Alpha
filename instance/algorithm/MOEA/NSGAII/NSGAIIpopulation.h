#ifndef NSGAII_POPULATION_H
#define NSGAII_POPULATION_H
#include <algorithm>

#include "../../../../core/algorithm/population.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace OFEC {

	// ----------------------------------------------------------------------
	// SimulatedBinaryCrossover : simulated binary crossover (SBX)
	// The implementation was adapted from the code of function realcross() in crossover.c
	// http://www.iitk.ac.in/kangal/codes/nsga2/nsga2-gnuplot-v1.1.6.tar.gz
	//
	// ref: http://www.slideshare.net/paskorn/simulated-binary-crossover-presentation#
	// ----------------------------------------------------------------------
	double get_betaq(double rand, double alpha, double ceta);

	template<typename Individual>
	void PolynomialMutation(Individual *indv, double mr, double meta) {
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

				y = y + deltaq*(ub - lb);
				y = std::min(ub, std::max(lb, y));

				x[i] = y;
			}
		}
	}

	template<typename Individual>
	void SimulatedBinaryCrossover(Individual *child1, Individual *child2, const Individual &parent1, const Individual &parent2, double cr, double ceta) {
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

			c1[i] = 0.5*((y1 + y2) - betaq*(y2 - y1));

			// child 2
			beta = 1.0 + (2.0*(ub - y2) / (y2 - y1));
			alpha = 2.0 - pow(beta, -(ceta + 1.0));
			betaq = get_betaq(rand, alpha, ceta);

			c2[i] = 0.5*((y1 + y2) + betaq*(y2 - y1));

			// boundary checking
			c1[i] = std::min(ub, std::max(lb, static_cast<double>(c1[i])));
			c2[i] = std::min(ub, std::max(lb, static_cast<double>(c2[i])));

			if (global::ms_global->m_uniform[caller::Algorithm]->next() <= 0.5)
			{
				std::swap(c1[i], c2[i]);
			}
		}
	}

	template<typename Individual>
	class NSGAIIpopulation : public population<Individual> {
	public:
		NSGAIIpopulation(int popsize) : population<Individual>(popsize, global::ms_global->m_problem->variable_size()), m_cr(1.0), m_ceta(30), m_meta(20){
			int numDim = global::ms_global->m_problem->variable_size();
			m_mr = 1.0 / numDim;
		}
		NSGAIIpopulation() : population<Individual>(), m_cr(1.0), m_ceta(30), m_meta(20) {
			int numDim = global::ms_global->m_problem->variable_size();
			m_mr = 1.0 / numDim;
		}
		~NSGAIIpopulation(){}
		void cross_mutate(const std::vector<int> &index, Individual *child1, Individual *child2) {
			SimulatedBinaryCrossover(child1, child2, *population<Individual>::m_pop[index[0]], *population<Individual>::m_pop[index[1]], m_cr, m_ceta);

			PolynomialMutation(child1, m_mr, m_meta);
			PolynomialMutation(child2, m_mr, m_meta);

			child1->evaluate();
			child2->evaluate();
		}
		void setCrossXP(double cr) { m_cr = cr; }
		void setMutationP(double mr) { m_mr = mr; }
		void setEta(double ceta, double meta) { m_ceta = ceta; m_meta = meta; }
	private:
		double m_cr;
		double m_ceta;
		double m_mr;
		double m_meta;
	};



	

	
}

#endif // !NSGAII_POPULATION_H
