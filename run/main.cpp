#define CATCH_CONFIG_RUNNER
#include "../utility/catch.hpp"

#include "initialize.h"

int main(int argc, char* argv[]) {

	time_t timer_start, timer_end;

	#ifndef OFEC_UNIT_TEST
		OFEC::set_global_parameters(argc, argv);
	#endif // !OFEC_UNIT_TEST

	OFEC::register_problem();
	OFEC::register_algoritm();
	time(&timer_start);
	//OFEC::run();
	int result = Catch::Session().run(argc, argv);

	time(&timer_end);
	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;

	return 0;
}
