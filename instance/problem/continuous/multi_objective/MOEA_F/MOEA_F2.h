#ifndef MOEA_F2_H
#define MOEA_F2_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F2 final : public MOEA_FBase
	{
	public:
		MOEA_F2(param_map &v);
		MOEA_F2(const std::string &name, size_t size_var);
		~MOEA_F2() {}
	};
}


#endif //MOEA_F2_H