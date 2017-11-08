#ifndef DTLZ4_H
#define DTLZ4_H

#include "DTLZ.h"

namespace OFEC {
	class DTLZ4 final : public DTLZ
	{
	public:
		DTLZ4(param_map &v);
		DTLZ4(const std::string &name, size_t size_var, size_t size_obj);
		~DTLZ4() {}; 
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //DTLZ4_H  