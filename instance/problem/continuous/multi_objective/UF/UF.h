#ifndef UF_H
#define UF_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class UF :public continuous {
	public:
		virtual void initialize();
	protected:
		UF(const std::string &name, size_t size_var, size_t size_obj);
		void load_PF();
	};
}

#endif
