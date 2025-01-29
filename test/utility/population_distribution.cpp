/********* Begin Register Information **********
{ 
	"description": "Space and transition of state of population distribution",
	"dependency on libraries": [ "Eigen" ]
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include "../../utility/population_distribution/state_space.h"
#include "../../utility/population_distribution/state_transition.h"
#include "../../instance/problem/continuous/single_objective/global/classical/sphere.h"
#include "../../instance/algorithm/template/classic/differential_evolution/population.h"
#include <iomanip>

using namespace ofec;

TEST_CASE("population to state ID") {
	size_t pop_size = 20, num_divs = 10, num_refs = 4, num_runs = 100;

	std::shared_ptr<Environment> env(Environment::create());
	env->recordInputParameters();
	env->initialize();
	env->setProblem(Sphere::create());
	env->problem()->recordInputParameters();
	env->initializeProblem();

	PopulationDE<> pop(num_divs, env.get());
	std::vector<const SolutionBase*> sols;
	for (size_t i = 0; i < pop.size(); ++i) {
		sols.push_back(&pop[i].solution());
	}
	Random rnd(0.5);
	population_distribution::StateSpace pdss(num_divs, env->problem());
	for (size_t i = 0; i < num_refs; ++i) {
		auto rnd_sol = env->problem()->createSolution();
		env->problem()->initializeVariables(rnd_sol->variableBase(), &rnd);
		pdss.addReferencePoint(*rnd_sol);
	}
	for (size_t i = 0; i < num_runs; i++) {
		pop.initialize(env.get(), &rnd);
		std::cout << std::setw(10) << std::right << pdss.solutionsToStateID(sols);
		for (size_t i = 1; i < pop.size(); ++i) {
			pop[i] = pop[0];
		}
		std::cout << std::setw(10) << std::right << pdss.solutionsToStateID(sols) << std::endl;
	}
}

TEST_CASE("transition probability") {
	size_t pop_size = 20, num_divs = 10, num_refs = 4, max_iters = 50, num_runs = 100;

	std::shared_ptr<Environment> env(Environment::create());
	env->recordInputParameters();
	env->initialize();
	env->setProblem(Sphere::create());
	env->problem()->recordInputParameters();
	env->initializeProblem();

	PopulationDE<> pop(num_divs, env.get());
	std::vector<const SolutionBase *> sols;
	for (size_t i = 0; i < pop.size(); ++i) {
		sols.push_back(&pop[i].solution());
	}
	Random rnd(0.5);
	population_distribution::StateSpace pdss(num_divs, env->problem());
	for (size_t i = 0; i < num_refs; ++i) {
		auto rnd_sol = env->problem()->createSolution();
		env->problem()->initializeVariables(rnd_sol->variableBase(), &rnd);
		pdss.addReferencePoint(*rnd_sol);
	}
	population_distribution::StateTransition pdst(pdss.numberStates());
	size_t id_from, id_to;
	for (size_t id_run = 0; id_run < num_runs; id_run++) {
		pop.initialize(env.get(), &rnd);
		pop.evaluate(env.get());
		id_to = pdss.solutionsToStateID(sols);
		for (size_t iter = 0; iter < max_iters; iter++) {
			pop.evolve(env.get(), &rnd);
			id_from = id_to;
			id_to = pdss.solutionsToStateID(sols);
			pdst.record(id_from, id_to);
		}
	}
	pdst.calculate();
	std::vector<size_t> ssids(pdst.sampledStatesIDs());
	std::sort(ssids.begin(), ssids.end());
	for (size_t i : ssids) {
		for (size_t j : ssids) {
			std::cout << std::setw(5) << std::fixed << std::setprecision(2) << pdst.probability(i, j);
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

TEST_CASE("convergence probability") {
	size_t pop_size = 20, num_divs = 10, num_refs = 4, max_iters = 50, num_runs = 100;

	std::shared_ptr<Environment> env(Environment::create());
	env->recordInputParameters();
	env->initialize();
	env->setProblem(Sphere::create());
	env->problem()->recordInputParameters();
	env->initializeProblem();

	PopulationDE<> pop(num_divs, env.get());
	std::vector<const SolutionBase *> sols;
	for (size_t i = 0; i < pop.size(); ++i) {
		sols.push_back(&pop[i].solution());
	}
	Random rnd(0.5);
	population_distribution::StateSpace pdss(num_divs, env->problem());
	for (size_t i = 0; i < num_refs; ++i) {
		auto rnd_sol = env->problem()->createSolution();
		env->problem()->initializeVariables(rnd_sol->variableBase(), &rnd);
		pdss.addReferencePoint(*rnd_sol);
	}
	population_distribution::StateTransition pdst(pdss.numberStates());
	size_t id_from, id_to;
	for (size_t id_run = 0; id_run < num_runs; id_run++) {
		pop.initialize(env.get(), &rnd);
		pop.evaluate(env.get());
		id_to = pdss.solutionsToStateID(sols);
		for (size_t iter = 0; iter < max_iters; iter++) {
			pop.evolve(env.get(), &rnd);
			id_from = id_to;
			id_to = pdss.solutionsToStateID(sols);
			pdst.record(id_from, id_to);
		}
	}
	pdst.calculate();
	pop.initialize(env.get(), &rnd);
	auto convergence_probability = pdst.convergenceProbability(pdss.solutionsToStateID(sols));
	std::vector<size_t> ssids(pdst.sampledStatesIDs());
	std::sort(ssids.begin(), ssids.end());
	for (size_t i : ssids) {
		std::cout << std::setw(5) << std::fixed << std::setprecision(2) <<
			convergence_probability[pdst.stateIDtoOrder(i)];
	}
	std::cout << std::endl;
}
