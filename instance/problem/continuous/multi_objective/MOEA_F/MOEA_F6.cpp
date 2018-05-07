#include "MOEA_F6.h"

namespace OFEC {
<<<<<<< HEAD
	MOEA_F6::MOEA_F6(param_map & v) : problem(v.at("proName"), v.at("numDim"), 3), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 3) {
		v.at("numObj") = 3;
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-2, 2));
		range[0].first = 0;
		range[0].second = 1;
		range[1].first = 0;
		range[1].second = 1;
		set_range(range);
		m_dtype = 1;
		m_ptype = 31;
		m_ltype = 32;
		LoadPF();
	}
	MOEA_F6::MOEA_F6(const std::string & name, size_t size_var) : problem(name, size_var, 3), \
		MOEA_FBase(name, size_var, 3) {
=======
	MOEA_F6::MOEA_F6(param_map & v) :
		MOEA_F6(v.at("proName"), v.at("numDim")) {
		
	}
	MOEA_F6::MOEA_F6(const std::string & name, size_t size_var) : problem(name, size_var, 3), \
		MOEA_FBase(name, size_var, 3) {
		
	}
	void MOEA_F6::initialize() {
>>>>>>> zhou_li
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-2, 2));
		range[0].first = 0;
		range[0].second = 1;
		range[1].first = 0;
		range[1].second = 1;
		set_range(range);
		m_dtype = 1;
		m_ptype = 31;
		m_ltype = 32;
<<<<<<< HEAD
		LoadPF();
=======
>>>>>>> zhou_li
	}
}