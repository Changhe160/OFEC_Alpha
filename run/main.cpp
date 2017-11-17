#include "initialize.h"

int main(int argc, char* argv[]) {

	time_t timer_start, timer_end;

	OFEC::set_global_parameters(argc, argv);
	OFEC::register_class();
	time(&timer_start);
	OFEC::run();
	time(&timer_end);
	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;

	return 0;
}