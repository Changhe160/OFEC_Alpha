#ifndef DTLZ3_H
#define DTLZ3_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ3 final : public DTLZ
	{
	public:
		DTLZ3(param_map &v);
		~DTLZ3() {};
	protected:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ3_H  