#include "individual.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/environment/environment.h"
#include "../../../../../utility/functional.h"

namespace ofec {
	IndividualDE::IndividualDE(Environment *env) : IndividualDE(
		env->problem()->numberObjectives(),
		env->problem()->numberConstraints(),
		env->problem()->numberVariables()) {}

	IndividualDE::IndividualDE(size_t num_objs, size_t num_cons, size_t num_vars) :
		Solution<>(num_objs, num_cons, num_vars),
		m_pv(num_objs, num_cons, num_vars),
		m_pu(num_objs, num_cons, num_vars),
		m_improved(true) {}

	IndividualDE::IndividualDE(const Solution<>& sol) :
		Solution<>(sol),
		m_pu(sol),
		m_pv(sol),
		m_improved(true) {}

	void IndividualDE::recombine(Real CR, de::RecombineStrategy RS, Random *rnd, Environment *env) {
		size_t dim = variable().size();
		if (RS == de::RecombineStrategy::kBinomial) {
			size_t I = rnd->uniform.nextNonStd<size_t>(0, dim);
			for (size_t i = 0; i < dim; ++i) {
				Real p = rnd->uniform.next();
				if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
				else m_pu.variable()[i] = variable()[i];
			}
		}
		else if (RS == de::RecombineStrategy::kExponential) {
			int n = rnd->uniform.nextNonStd<int>(0, dim);
			size_t L = 0;
			do {
				L = L + 1;
			} while (rnd->uniform.next() < CR && L < dim);
			for (size_t i = 0; i < dim; i++) {
				if (i < L)
					m_pu.variable()[(n + i) % dim] = m_pv.variable()[(n + i) % dim];
				else
					m_pu.variable()[(n + i) % dim] = variable()[(n + i) % dim];
			}
		}
		env->problem()->validateSolution(m_pu, Validation::kSetToBound, rnd);
	}

	void IndividualDE::mutate(Real F,
		const Solution<> *s1,
		const Solution<> *s2,
		const Solution<> *s3,
		Environment *env,
		const Solution<> *s4,
		const Solution<> *s5,
		const Solution<>* s6,
		const Solution<>* s7) 
	{
		Real l, u;
		for (size_t i = 0; i < m_pv.variable().size(); ++i) {
			l = CAST_CONOP(env->problem())->range(i).first;
			u = CAST_CONOP(env->problem())->range(i).second;
			m_pv.variable()[i] = (s1->variable()[i]) + F * ((s2->variable()[i]) - (s3->variable()[i]));
			if (s4 && s5) m_pv.variable()[i] += F * ((s4->variable()[i]) - (s5->variable()[i]));
			if (s6 && s7) m_pv.variable()[i] += F * ((s6->variable()[i]) - (s7->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndividualDE::mutate(Real F,
		const std::vector<int>& var,
		const Solution<> *s1,
		const Solution<> *s2,
		const Solution<> *s3,
		Environment *env,		 
		const Solution<> *s4,
		const Solution<> *s5,
		const Solution<>* s6,
		const Solution<>* s7)
	{
		Real l, u;
		for (auto i : var) {
			l = CAST_CONOP(env->problem())->range(i).first;
			u = CAST_CONOP(env->problem())->range(i).second;
			m_pv.variable()[i] = (s1->variable()[i]) + F * ((s2->variable()[i]) - (s3->variable()[i]));
			if (s4 && s5) m_pv.variable()[i] += F * ((s4->variable()[i]) - (s5->variable()[i]));
			if (s6 && s7) m_pv.variable()[i] += F * ((s6->variable()[i]) - (s7->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndividualDE::mutate(Real K, Real F, 
		const Solution<>* s1, 
		const Solution<>* s2, 
		const Solution<>* s3, 
		const Solution<>* s4, 
		const Solution<>* s5, 
		Environment *env)
	{
		Real l, u;
		for (size_t i = 0; i < m_pu.variable().size(); ++i) {
			l = CAST_CONOP(env->problem())->range(i).first;
			u = CAST_CONOP(env->problem())->range(i).second;
			m_pu.variable()[i] = (s1->variable()[i]) + K * ((s2->variable()[i]) - (s3->variable()[i]))+ F * ((s4->variable()[i]) - (s5->variable()[i]));
			if ((m_pu.variable()[i]) > u) {
				m_pu.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pu.variable()[i]) < l) {
				m_pu.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndividualDE::recombine(Real CR, const std::vector<int>& var, int I, Random *rnd) {
		for (auto i : var) {
			Real p = rnd->uniform.next();
			if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
			else m_pu.variable()[i] = variable()[i];
		}
	}

	int IndividualDE::select(Environment *env) {
		int tag = m_pu.evaluate(env);
		if(m_pu.dominate(*this,env)){
			variable() = m_pu.variable();
			m_objectives = m_pu.objective();
			m_constraints = m_pu.constraint();
			m_improved = true;
		}
		else {
			m_improved = false;
		}
		return tag;
	}
}
