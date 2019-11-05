#include "logistic_regression.h"

namespace OFEC {
	void check_it(bool flag) {
		if (!flag) fprintf(stderr, "logistic regression error! \n");
	}
}