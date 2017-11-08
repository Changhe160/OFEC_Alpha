#ifndef DTLZ2_H
#define DTLZ2_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ2 final : public DTLZ
	{
	public:
		DTLZ2(param_map &v);
		DTLZ2(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ2() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ2_H  