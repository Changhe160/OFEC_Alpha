#ifndef DTLZ1_H
#define DTLZ1_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ1 final : public DTLZ
	{
	public:
		DTLZ1(param_map &v);
		DTLZ1(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ1() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ1_H  