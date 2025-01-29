#ifndef OFEC_DE_DEFINITION_H
#define OFEC_DE_DEFINITION_H

namespace ofec::de {
	enum class MutateStrategy {
		kRand1,
		kBest1,
		kCurrentToBest1,
		kBest2,
		kRand2,
		kRandToBest1,
		kCurrentToRand1
	};
	
	enum class RecombineStrategy {
		kBinomial, 
		kExponential
	};
}

#endif // !OFEC_DE_DEFINITION_H
