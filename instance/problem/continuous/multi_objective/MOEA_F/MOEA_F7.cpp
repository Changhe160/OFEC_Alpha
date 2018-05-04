#include "MOEA_F7.h"

namespace OFEC {
	MOEA_F7::MOEA_F7(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		
	}
	MOEA_F7::MOEA_F7(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
	
	}
	void MOEA_F7::initialize() {
		MOEA_FBase::initialize();
		m_dtype = 3;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}