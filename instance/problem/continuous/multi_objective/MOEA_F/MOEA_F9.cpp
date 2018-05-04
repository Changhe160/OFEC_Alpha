#include "MOEA_F9.h"

namespace OFEC {
	MOEA_F9::MOEA_F9(param_map & v) : problem(v.at("proName"), v.at("numDim"), 2), \
		MOEA_FBase(v.at("proName"), v.at("numDim"), 2) {
		
	}
	MOEA_F9::MOEA_F9(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F9::initialize() {
		MOEA_FBase::initialize();
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		m_dtype = 1;
		m_ptype = 22;
		m_ltype = 22;
		LoadPF();
	}
}