#ifndef DTLZ2_H
#define DTLZ2_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ2 final : public DTLZ
	{
	public:
		DTLZ2(param_map &v);
		~DTLZ2() {};
	protected:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ2_H  