#define CATCH_CONFIG_RUNNER
#include "../utility/catch.hpp"

#include "system_initialization.h"
#include "user_initialization.h"
#include "register_algorithm.h"
#include "register_problem.h"

int main(int argc, char* argv[]) {

	time_t timer_start, timer_end;

#ifndef OFEC_UNIT_TEST
	OFEC::set_global_parameters(argc, argv);
#else
	OFEC::register_parameter();
#endif // !OFEC_UNIT_TEST

	OFEC::register_problem();
	OFEC::register_algorithm();
	OFEC::set_alg4pro();
	if (!OFEC::check_validation()) return 0;
	time(&timer_start);

#ifdef OFEC_UNIT_TEST
	int result = Catch::Session().run(argc, argv);
#else
	OFEC::run();
#endif // OFEC_UNIT_TEST

	time(&timer_end);
	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;

	return 0;
}