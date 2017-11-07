#ifndef MOEA_F8_H
#define MOEA_F8_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F8 final : public MOEA_FBase
	{
	public:
		MOEA_F8(param_map &v);
		MOEA_F8(const std::string &name, size_t size_var);
		~MOEA_F8() {}
	};
}


#endif //MOEA_F8_H