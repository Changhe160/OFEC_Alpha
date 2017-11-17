#ifndef MOEA_F4_H
#define MOEA_F4_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F4 final : public MOEA_FBase
	{
	public:
		MOEA_F4(param_map &v);
		MOEA_F4(const std::string &name, size_t size_var);
		~MOEA_F4() {}
	};
}


#endif //MOEA_F4_H