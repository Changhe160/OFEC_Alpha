#include "MOEA_F8.h"

namespace OFEC {
<<<<<<< HEAD
	MOEA_F8::MOEA_F8(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		v.at("numObj") = 2;
		m_dtype = 4;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
	MOEA_F8::MOEA_F8(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		m_dtype = 4;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
=======
	MOEA_F8::MOEA_F8(param_map & v) :
		MOEA_F8(v.at("proName"), v.at("numDim")) {
		
	}
	MOEA_F8::MOEA_F8(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F8::initialize() {
		m_dtype = 4;
		m_ptype = 21;
		m_ltype = 21;
>>>>>>> zhou_li
	}
}