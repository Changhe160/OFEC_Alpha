#include "MOEA_F4.h"

namespace OFEC {
<<<<<<< HEAD
	MOEA_F4::MOEA_F4(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		v.at("numObj") = 2;
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 24;
		LoadPF();
	}
	MOEA_F4::MOEA_F4(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
=======
	MOEA_F4::MOEA_F4(param_map & v) : 
		MOEA_F4(v.at("proName"), v.at("numDim")) {
		
	}
	MOEA_F4::MOEA_F4(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F4::initialize() {
>>>>>>> zhou_li
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 24;
<<<<<<< HEAD
		LoadPF();
=======
>>>>>>> zhou_li
	}
}