#include "individual.h"

namespace OFEC {
	namespace DE {
        individual::individual(size_t num_obj, size_t num_con, size_t size_var) :
                OFEC::individual<>(num_obj, num_con, size_var),
                m_pv(num_obj, num_con, size_var),
                m_pu(num_obj, num_con, size_var) {}

		individual::individual(const solution<>& sol) : OFEC::individual<>(sol), m_pu(sol), m_pv(sol){}

//       individual::individual(const individual &rhs) : OFEC::individual<>(rhs),  m_pv(rhs), m_pu(rhs) {}

//       individual::individual(individual &&rhs) noexcept :
//                OFEC::individual<>(std::move(rhs)),
//                m_pv(std::move(rhs.m_pv)),
//                m_pu(std::move(rhs.m_pu)) {}

//		individual& individual::operator=(const individual &rhs) {
//			if (this == &rhs) return *this;
//			OFEC::individual<>::operator=(rhs);
//			m_pv = rhs.m_pv;
//			m_pu = rhs.m_pu;
//			return *this;
//		}

//		individual& individual::operator=(individual &&rhs) noexcept {
//			if (this == &rhs) return *this;
//			m_pv = std::move(rhs.m_pv);
//			m_pu = std::move(rhs.m_pu);
//			OFEC::individual<>::operator=(std::move(rhs));
//			return *this;
//		}

		void individual::initialize(int id) {
			OFEC::individual<>::initialize(id);
			m_pu.variable() = variable();
			m_pu.objective() = objective();
			m_pu.constraint_value() = constraint_value();
			m_pv.variable() = variable();
			m_pv.objective() = objective();
			m_pv.constraint_value() = constraint_value();
		}

		void individual::mutate(real F,
			solution<> *r1,
			solution<> *r2,
			solution<> *r3,
			solution<> *r4,
			solution<> *r5) {
			real l, u;
			for (size_t i = 0; i < m_pv.variable().size(); ++i) {
				l = CONTINUOUS_CAST->range(i).first;
				u = CONTINUOUS_CAST->range(i).second;
				m_pv.variable()[i] = (r1->variable()[i]) + F * ((r2->variable()[i]) - (r3->variable()[i]));
				if (r4&&r5) m_pv.variable()[i] += F * ((r4->variable()[i]) - (r5->variable()[i]));

				if ((m_pv.variable()[i]) > u) {
					m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
				}
				else if ((m_pv.variable()[i]) < l) {
					m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
				}
			}
		}

		void individual::recombine(real CR, recombine_strategy rs) {
			size_t dim = m_var.size();
			if (rs == recombine_strategy::binomial) {
				size_t I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, dim);
				for (size_t i = 0; i < dim; ++i) {
					real p = global::ms_global->m_uniform[caller::Algorithm]->next();
					if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
					else m_pu.variable()[i] = variable()[i];
				}
			}
			else if (rs == recombine_strategy::exponential) {
				int n = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, dim);
				size_t L = 0;
				do {
					L = L + 1;
				} while (global::ms_global->m_uniform[caller::Algorithm]->next() < CR && L < dim);
				for (size_t i = 0; i < dim; i++) {
					if (i < L)
						m_pu.variable()[(n + i) % dim] = m_pv.variable()[(n + i) % dim];
					else
						m_pu.variable()[(n + i) % dim] = m_var[(n + i) % dim];
				}
			}
		}

		void individual::mutate(real F, const std::vector<int> &var,
			solution<> *r1,
			solution<> *r2,
			solution<> *r3,
			solution<> *r4,
			solution<> *r5) {
			real l, u;
			for (auto &i : var) {
				l = CONTINUOUS_CAST->range(i).first;
				u = CONTINUOUS_CAST->range(i).second;
				m_pv.variable()[i] = (r1->variable()[i]) + F * ((r2->variable()[i]) - (r3->variable()[i]));
				if (r4&&r5) m_pv.variable()[i] += F * ((r4->variable()[i]) - (r5->variable()[i]));
				if ((m_pv.variable()[i]) > u) {
					m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
				}
				else if ((m_pv.variable()[i]) < l) {
					m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
				}
			}
		}

		void individual::recombine(real CR, const std::vector<int> &var, int I) {
			for (auto &i : var) {
				real p = global::ms_global->m_uniform[caller::Algorithm]->next();
				if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
				else m_pu.variable()[i] = variable()[i];
			}
		}

		evaluation_tag individual::select() {
			evaluation_tag tag = m_pu.evaluate();
			if (m_pu.dominate(*this)) {
				m_var = m_pu.variable();
				m_obj = m_pu.objective();
				m_constraint_value = m_pu.constraint_value();
				m_improved = true;
			}
			else {
				m_improved = false;
			}
			return tag;
		}

		//evaluation_tag individual::select(const std::vector<int> &var, solution<> &best) {
		//	int dim = m_var.size();
		//	solution<> t(m_pu);
		//	for (int i = 0, j = 0; i < dim; ++i) {
		//		if (find(var.begin(), var.end(), i) == var.end()) {
		//			t.variable()[i] = best.variable()[i];
		//		}
		//		else {
		//			t.variable()[i] = m_pu.variable()[j++];
		//		}
		//	}
		//	evaluation_tag tag = t.evaluate();
		//	m_pu.objective() = t.objective();
		//	if (m_pu.dominate(*this)) {
		//		*this = m_pu;
		//		if (t.dominate(best))
		//			best = t;
		//	}
		//	return tag;
		//}

		solution<>& individual::trial() {
			return m_pu;
		}
    }
}