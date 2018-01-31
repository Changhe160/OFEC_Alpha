#ifndef OFEC_DYNDESUBPOP_H
#define OFEC_DYNDESUBPOP_H
#include "DynDEIndividual.h"
#include "../DEpopulation.h"
namespace OFEC {
	class DynDE;
	class DynDESubPop :public DEpopulation<DynDEindividual> {
	public:
		bool m_flag;
	public:
		friend class DynDE;
		DynDESubPop(size_t size, size_t dim);
		void assign_type();
	
		evaluation_tag evolve();
	private:
		int m_num_normal;    // the number of normal individuals of each swarm
		int m_num_brownian;       // the number of brownian individuals of each swarm
		int m_num_quantum;    // the number of quantum individuals of each swarm
		double m_r_cloud; // radius of quantum swarms
		double m_sigma;	// deviation for generation of Brownian individuals
	};

}
#endif //OFEC_DYNDESUBPOP_H