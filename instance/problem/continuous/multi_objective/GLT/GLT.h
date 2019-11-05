#ifndef GLT_H
#define GLT_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class GLT :public continuous {
	public:
		virtual void initialize();
	protected:
		GLT(const std::string &name, size_t size_var, size_t size_obj);
		void load_PF();
		//void generateAdLoadPF();
	};
}

#endif

