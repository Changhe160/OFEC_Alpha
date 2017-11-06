#include "MOEA_F1.h"

namespace OFEC {
	MOEA_F1::MOEA_F1(param_map & v) : problem(v[param_proName], v[param_numDim], 2), \
		MOEA_FBase(v[param_proName], v[param_numDim], 2) {
		global::ms_arg[param_numObj] = 2;
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}