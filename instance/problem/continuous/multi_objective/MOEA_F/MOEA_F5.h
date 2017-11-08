#ifndef MOEA_F5_H
#define MOEA_F5_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F5 final : public MOEA_FBase
	{
	public:
		MOEA_F5(param_map &v);
		MOEA_F5(const std::string &name, size_t size_var);
		~MOEA_F5() {}
	};
}


#endif //MOEA_F5_H