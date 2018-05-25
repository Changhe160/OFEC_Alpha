#ifndef INCLUDE_H
#define INCLUDE_H

#include "user_initialization.h"

namespace OFEC {

	void set_global_parameters(int argn, char *argv[]);
	void run();
	int go(std::vector<int> runIds);
	void set_default_filename_info();
	void add_filename_info(const std::vector<std::string>& info);
	void set_alg4pro();
	bool check_validation();
}

#endif // !INCLUDE_H
