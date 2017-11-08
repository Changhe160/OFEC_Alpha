#ifndef MOEA_F9_H
#define MOEA_F9_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F9 final : public MOEA_FBase
	{
	public:
		MOEA_F9(param_map &v);
		MOEA_F9(const std::string &name, size_t size_var);
		~MOEA_F9() {}
	};
}


#endif //MOEA_F9_H