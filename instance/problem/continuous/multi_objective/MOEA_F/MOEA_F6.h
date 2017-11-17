#ifndef MOEA_F6_H
#define MOEA_F6_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F6 final : public MOEA_FBase
	{
	public:
		MOEA_F6(param_map &v);
		MOEA_F6(const std::string &name, size_t size_var);
		~MOEA_F6() {}
	};
}


#endif //MOEA_F6_H