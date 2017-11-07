#include "DG_NSGAII.h"

namespace OFEC {
	void DG_NSGAII::sort() {
		size_t offspring_size = m_offspring.size();
		if (m_DG_empty) {
			for (size_t i = 0; i < offspring_size; ++i) {
				// add new node
				m_offspring[i].set_DG_pos(DG.insert(m_offspring[i].get_objective()));
				m_offspring[i].set_id(1); // id = 1 means node already in dominance graph 
			}
			m_DG_empty = false;
			std::map<int, int> ranking = DG.ranking();
			for (size_t i = 0; i < offspring_size; ++i)
				m_offspring[i].set_rank(ranking[m_offspring[i].DG_pos()]);
			m_objcomp = DG.number();
			return;
		}
		// delete changed node
		std::vector<bool> changed(offspring_size, true);
		for (int i = 0; i < m_parent.size(); ++i) {
			changed[m_parent[i].DG_pos()] = false;
		}
		for (size_t i = 0; i < changed.size(); ++i) {
			if (changed[i]) {
				DG._delete(i);
				for (size_t j = 0; j < offspring_size; ++j) {
					if (m_offspring[j].id() == 1 && m_offspring[j].DG_pos() > i)
						m_offspring[j].set_DG_pos(m_offspring[j].DG_pos() - 1);
				}
				for (size_t j = i + 1; j < changed.size(); ++j) {
					changed[j - 1] = changed[j];
				}
				changed.pop_back();
				--i;
			}
		}
		// add changed node
		for (size_t i = 0; i < offspring_size; ++i) {
			if (m_offspring[i].id() == 0) {
				m_offspring[i].set_DG_pos(DG.insert(m_offspring[i].get_objective()));
				m_offspring[i].set_id(1);
			}
		}
		// output sorting result
		std::map<int, int> ranking = DG.ranking();
		for (size_t i = 0; i < offspring_size; ++i)
			m_offspring[i].set_rank(ranking[m_offspring[i].DG_pos()]);
		m_objcomp = DG.number();
	}
	void DG_NSGAII::evolve_mo() {
		int m = m_parent.size();
		if (m_parent.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAII::evolve_mo()");
		for (int n = 0; n<m_parent.size(); n += 2)
		{
			m_offspring[n] = m_parent[n];
			m_offspring[n + 1] = m_parent[n + 1];
			if ((m + 1) < m_offspring.size()) {
				std::vector<int> p(2);
				p[0] = tour_selection();
				while (1) { p[1] = tour_selection();  	if (p[1] != p[0]) break; }
				m_parent.cross_mutate(p, &m_offspring[m], &m_offspring[m + 1]);
				m_offspring[m].set_id(0); //  id = 0 means node value changed
				m_offspring[m + 1].set_id(0);
			}
			else if ((m + 1) > m_offspring.size()) {

			}
			else {
				std::vector<int> p(2);
				p[0] = tour_selection();
				while (1) { p[1] = tour_selection();  	if (p[1] != p[0]) break; }
				m_parent.cross_mutate(p, &m_offspring[m], &m_offspring[m]);
				m_offspring[m].set_id(0); //  id = 0 means node value changed
			}
			m += 2;
		}
	}
}
