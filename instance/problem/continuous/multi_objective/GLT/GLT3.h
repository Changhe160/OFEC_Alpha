#ifndef GLT3_H
#define GLT3_H


#include "GLT.h"

namespace OFEC {
	class GLT3 final : public GLT {
	public:
		GLT3(param_map &v);
		GLT3(const std::string &name, size_t size_var);
		~GLT3() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif