#include "DynDE_subpopulation.h"
namespace OFEC {
	DynDE_subpopulation::DynDE_subpopulation(size_t size) : DE::population<DynDE_individual>(size), m_num_normal(5), m_num_brownian(5), m_num_quantum(5), \
		m_r_cloud(1.0), m_sigma(0.2) {
		// set m_Rcloud to shift lenght, default for shift length =1.0 if not known 
		//default configuration (N,N+) or (N,Nq)= (5,5)
		assign_type();
		set_mutation_strategy(DE::mutation_strategy::best_2);
		//update_archive(*m_inds[0]);
	}

	void DynDE_subpopulation::assign_type() {

		// for the version with the suggested settings 
		for (size_t i = 0; i < size(); ++i) {
			if (i < m_num_normal) m_inds[i]->m_type = DynDE_individual::individual_type::TYPE_DE;
			else  m_inds[i]->m_type = DynDE_individual::individual_type::TYPE_BROWNIAN;
		}

	}


	evaluation_tag DynDE_subpopulation::evolve() {
		if (this->size() < 1)		 return evaluation_tag::Normal;

		evaluation_tag tag = evaluation_tag::Normal;
		for (size_t i = 0; i < this->size(); ++i) {
			if (m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_DE || m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_ENTROPY_DE) {
				mutate(i);
				this->m_inds[i]->recombine(m_CR, m_recombine_strategy);
			}
			else if (m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_BROWNIAN) {
				tag = m_inds[i]->brownian(*m_best[0], m_sigma);

			}
			else if (m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_QUANTUM) {
				tag = m_inds[i]->quantum(*m_best[0], m_r_cloud);
			}
			if (tag != evaluation_tag::Normal)  return tag;
		}

		//this->updateIDnIndex();


		for (int i = 0; i < this->size(); i++) {
			if (m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_DE || m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_ENTROPY_DE) {
				tag = m_inds[i]->select();
				if (tag != evaluation_tag::Normal) return tag;
			}

			if (m_inds[i]->m_type == DynDE_individual::individual_type::TYPE_ENTROPY_DE) {
				// add entropy if necessary 
				tag = m_inds[i]->entropy(m_sigma);
				if (tag != evaluation_tag::Normal) return tag;
			}
			//update_archive(*m_inds[i]);
		}
		if (tag == evaluation_tag::Normal) {
			this->m_iter++;
		}
		return tag;
	}
}
