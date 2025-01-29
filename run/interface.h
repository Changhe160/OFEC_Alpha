#ifndef OFEC_INTERFACE_H
#define OFEC_INTERFACE_H

#include <string>
#include <memory>
#include <set>
#include <map>
#include "../core/environment/environment.h"
#include "factory.h"

namespace ofec {
	extern Factory<Environment> g_factory_environment;
	extern Factory<Problem> g_factory_problem;
	extern Factory<Algorithm> g_factory_algorithm;
	extern std::map<std::string, std::set<std::string>> g_algorithm_for_environment_problem;
	void registerInstance();
	bool checkValidation(
		const std::string &environment_problem_name, 
		const std::string &algorithm_name
	);
	Environment* generateEnvironmentByFactory(const std::string &environment_problem_name);
	Problem* generateProblemByFactory(const std::string &environment_problem_name);
	Algorithm* generateAlgorithmByFactory(const std::string &algorithm_name);
}

#endif // !OFEC_INTERFACE_H
