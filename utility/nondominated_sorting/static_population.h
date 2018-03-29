#ifndef STATIC_POPULATION_H
#define STATIC_POPULATION_H

#include "../vector.h"
#include "../random/newran.h"

namespace NDS {
	class uniform_population {
	public:
		uniform_population(const int obj_num, const int pop_size, const double rand_seed) :m_obj_num(obj_num), m_pop_size(pop_size), m_rand(rand_seed){}
		std::vector<std::vector<double>> generate_new(const int rank_num);
		void generate_output(const int rank_num);
	private:
		const int m_obj_num;
		const int m_pop_size;
		OFEC::uniform m_rand;
	};
}

#endif // !STATIC_POPULATION_H

