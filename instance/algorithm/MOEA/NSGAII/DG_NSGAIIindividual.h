#ifndef DG_NSGAIIINDIVIDUAL_H
#define DG_NSGAIIINDIVIDUAL_H

#include "../../../../core/algorithm/individual.h"

namespace OFEC {
	class DG_NSGAIIindividual : public individual<> {
	public:
		template<typename ... Args>
		DG_NSGAIIindividual(size_t no, Args&& ... args) : individual(no, std::forward<Args>(args)...){}
		DG_NSGAIIindividual& operator=(const DG_NSGAIIindividual& rhs) {
			if (this == &rhs) return *this;
			individual::operator=(rhs);
			m_DG_pos = rhs.m_DG_pos;
			return *this;
		}
		void set_DG_pos(int pos) { m_DG_pos = pos; }
		int DG_pos() const { return m_DG_pos; }
	protected:
		int m_DG_pos;
	};
}

#endif // !DG_NSGAIIINDIVIDUAL_H


