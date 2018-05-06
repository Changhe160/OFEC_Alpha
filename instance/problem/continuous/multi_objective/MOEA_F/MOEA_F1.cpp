#include "MOEA_F1.h"

namespace OFEC {
	MOEA_F1::MOEA_F1(param_map & v) :
		MOEA_F1(v.at("proName"), v.at("numDim")) {
		
	}
	MOEA_F1::MOEA_F1(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2){
		
	}
	void MOEA_F1::initialize() {
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 21;
	}
}