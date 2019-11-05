#ifndef GLT1_H
#define GLT1_H


#include "GLT.h"

namespace OFEC {
	class GLT1 final : public GLT {
	public:
		GLT1(param_map &v);
		GLT1(const std::string &name, size_t size_var);
		~GLT1() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif