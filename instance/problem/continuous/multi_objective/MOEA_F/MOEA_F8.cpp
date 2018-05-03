#include "MOEA_F8.h"

namespace OFEC {
	MOEA_F8::MOEA_F8(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		
	}
	MOEA_F8::MOEA_F8(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F8::initialize_problem() {
		MOEA_FBase::initialize_problem();
		m_dtype = 4;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}