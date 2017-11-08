#ifndef MOEA_F7_H
#define MOEA_F7_H

#include "MOEA_F.h"

namespace OFEC {
	class MOEA_F7 final : public MOEA_FBase
	{
	public:
		MOEA_F7(param_map &v);
		MOEA_F7(const std::string &name, size_t size_var);
		~MOEA_F7() {}
	};
}


#endif //MOEA_F7_H