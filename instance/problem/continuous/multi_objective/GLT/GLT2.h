#ifndef GLT2_H
#define GLT2_H


#include "GLT.h"

namespace OFEC {
	class GLT2 final : public GLT {
	public:
		GLT2(param_map &v);
		GLT2(const std::string &name, size_t size_var);
		~GLT2() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif
