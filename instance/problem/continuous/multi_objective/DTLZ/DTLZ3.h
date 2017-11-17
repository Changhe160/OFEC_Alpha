#ifndef DTLZ3_H
#define DTLZ3_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ3 final : public DTLZ
	{
	public:
		DTLZ3(param_map &v);
		DTLZ3(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ3() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ3_H  