#ifndef MOEA_F1_H
#define MOEA_F1_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F1 final : public MOEA_FBase
	{
	public:
		MOEA_F1(param_map &v);
		MOEA_F1(const std::string &name, size_t size_var);
		~MOEA_F1() {}
	};
}


#endif //MOEA_F1_H