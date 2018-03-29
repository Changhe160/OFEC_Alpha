#ifndef NEW_STATIC_POPULATION_H
#define NEW_STATIC_POPULATION_H

#include <vector>
#include "../random/newran.h"

namespace NDS {
	class new_uniform_population {
	public:
		new_uniform_population(const int obj_num, const int pop_size, const double rand_seed) :m_obj_num(obj_num), m_pop_size(pop_size), m_rand(rand_seed) {}
		std::vector<std::vector<double>> generate_new(const int rank_num);
		std::vector<std::vector<double>> generate_output(const int rank_num);
	private:
		const int m_obj_num;
		const int m_pop_size;
		OFEC::uniform m_rand;
	};
}

#endif // !NEW_STATIC_POPULATION_H

