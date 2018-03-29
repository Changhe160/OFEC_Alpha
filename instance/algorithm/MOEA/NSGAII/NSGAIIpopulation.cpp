#include "NSGAIIpopulation.h"

namespace OFEC {
	double get_betaq(double rand, double alpha, double ceta) {
		double betaq = 0.0;
		if (rand <= (1.0 / alpha))
		{
			betaq = pow((rand*alpha), (1.0 / (ceta + 1.0)));
		}
		else
		{
			betaq = pow((1.0 / (2.0 - rand*alpha)), (1.0 / (ceta + 1.0)));
		}
		return betaq;
	}
}