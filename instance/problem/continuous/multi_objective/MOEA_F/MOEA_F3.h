#ifndef MOEA_F3_H
#define MOEA_F3_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F3 final : public MOEA_FBase
	{
	public:
		MOEA_F3(param_map &v);
		MOEA_F3(const std::string &name, size_t size_var);
		~MOEA_F3() {}
	};
}


#endif //MOEA_F3_H