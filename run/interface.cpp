#include "interface.h"

namespace ofec {
	Factory<Environment> g_factory_environment;
	Factory<Problem> g_factory_problem;
	Factory<Algorithm> g_factory_algorithm;
	std::map<std::string, std::set<std::string>> g_algorithm_for_environment_problem;

	bool checkValidation(
		const std::string &environment_problem_name,
		const std::string &algorithm_name
	) {
		if (g_algorithm_for_environment_problem.count(environment_problem_name) &&
			g_algorithm_for_environment_problem.at(environment_problem_name).count(algorithm_name))
		{
			return true;
		}
		else {
			return false;
		}
	}

	Environment* generateEnvironmentByFactory(const std::string &environment_problem_name) {
		if (g_factory_environment.get().count(environment_problem_name)) {
			return Factory<Environment>::produce(environment_problem_name);
		}
		else {
			return Environment::create();
		}
	}

	Problem* generateProblemByFactory(const std::string &environment_problem_name) {
		return Factory<Problem>::produce(environment_problem_name);
	}

	Algorithm* generateAlgorithmByFactory(const std::string &algorithm_name) {
		return Factory<Algorithm>::produce(algorithm_name);
	}
}