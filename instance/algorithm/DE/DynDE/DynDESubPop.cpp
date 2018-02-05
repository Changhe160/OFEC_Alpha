#include "DynDESubPop.h"
namespace OFEC {
	DynDESubPop::DynDESubPop(size_t size, size_t dim) :DEpopulation<DynDEindividual>(size, dim), m_num_normal(5), m_num_brownian(5), m_num_quantum(5), \
		m_r_cloud(1.0), m_sigma(0.2) {
		// set m_Rcloud to shift lenght, default for shift length =1.0 if not known 
		//default configuration (N,N+) or (N,Nq)= (5,5)
		assign_type();
		setMutationStrategy(DE_best_2);
	}
	void DynDESubPop::assign_type() {

		// for the version with the suggested settings 
		for (size_t i = 0; i < size(); ++i) {
			if (i < m_num_normal) m_pop[i]->m_type = DynDEindividual::IndividualType::TYPE_DE;
			else  m_pop[i]->m_type = DynDEindividual::IndividualType::TYPE_BROWNIAN;
		}

	}


	evaluation_tag DynDESubPop::evolve() {
		if (this->size() < 1)		 return evaluation_tag::Normal;

		evaluation_tag tag = evaluation_tag::Normal;
		for (size_t i = 0; i < this->size(); ++i) {
			if (m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_DE || m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_ENTROPY_DE) {
				mutate(i);
				this->m_pop[i]->recombine(m_CR);
			}
			else if (m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_BROWNIAN) {
				tag = m_pop[i]->brownian(*m_best[0], m_sigma);

			}
			else if (m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_QUANTUM) {
				tag = m_pop[i]->quantum(*m_best[0], m_r_cloud);
			}
			if (tag != evaluation_tag::Normal)  return tag;
		}

		//this->updateIDnIndex();


		for (int i = 0; i < this->size(); i++) {
			if (m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_DE || m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_ENTROPY_DE) {
				tag = m_pop[i]->select();
				if (tag != evaluation_tag::Normal) return tag;
			}

			if (m_pop[i]->m_type == DynDEindividual::IndividualType::TYPE_ENTROPY_DE) {
				// add entropy if necessary 
				tag = m_pop[i]->entropy(m_sigma);
				if (tag != evaluation_tag::Normal) return tag;
			}
			update_archive(*m_pop[i]);
		}
		if (tag == evaluation_tag::Normal) {
			this->m_iter++;
		}
		return tag;
	}
}
