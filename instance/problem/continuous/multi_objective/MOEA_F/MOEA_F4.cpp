#include "MOEA_F4.h"

namespace OFEC {
	MOEA_F4::MOEA_F4(param_map & v) : 
		MOEA_F4(v.at("proName"), v.at("numDim")) {
		
	}
	MOEA_F4::MOEA_F4(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F4::initialize() {
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		set_init_range(range);
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Minimization;
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 24;
		LoadPF();
	}
}