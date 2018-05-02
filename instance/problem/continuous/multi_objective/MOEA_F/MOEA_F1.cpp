#include "MOEA_F1.h"

namespace OFEC {
	MOEA_F1::MOEA_F1(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		v.at("numObj") = 2;
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
	MOEA_F1::MOEA_F1(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2){
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}