#include "global.h"

namespace OFEC {
	
#ifdef OFEC_CONSOLE
	thread_local std::unique_ptr<global> global::ms_global = nullptr;
#endif // OFEC_CONSOLE

#ifdef OFEC_DEMON
	std::unique_ptr<global> global::ms_global = nullptr;
#endif


#ifdef USING_GPU
	compute::device global::m_device = compute::system::default_device();
#endif // USING_GPU

	param_map global::ms_arg;

	global::global() {
		double seed = 0.5;

		m_uniform.emplace(make_pair(caller::Problem, std::unique_ptr<uniform>(new uniform(seed))));
		m_normal.emplace(make_pair(caller::Problem, std::unique_ptr<normal>(new normal(seed))));
		m_cauchy.emplace(make_pair(caller::Problem, std::unique_ptr<cauchy>(new cauchy(seed))));
		m_levy.emplace(make_pair(caller::Problem, std::unique_ptr<levy>(new levy(1.4, seed))));
		m_gamma.emplace(make_pair(caller::Problem, std::unique_ptr<gamma>(new gamma(0.5, seed))));

		

		m_uniform.emplace(make_pair(caller::Algorithm, std::unique_ptr<uniform>(new uniform(seed))));
		m_normal.emplace(make_pair(caller::Algorithm, std::unique_ptr<normal>(new normal(seed))));
		m_cauchy.emplace(make_pair(caller::Algorithm, std::unique_ptr<cauchy>(new cauchy(seed))));
		m_levy.emplace(make_pair(caller::Algorithm, std::unique_ptr<levy>(new levy(1.4, seed))));
		m_gamma.emplace(make_pair(caller::Algorithm, std::unique_ptr<gamma>(new gamma(0.5, seed))));
	}

	global::global(double seed_pro , double seed_alg ) : m_problem(nullptr), m_algorithm(nullptr) {
		double seed = 0;
		if (seed_pro<0 || seed_pro>1) seed = 0.5;
		else seed = seed_pro;
		
		m_uniform.emplace(make_pair(caller::Problem, std::unique_ptr<uniform>(new uniform(seed))));
		m_normal.emplace(make_pair(caller::Problem, std::unique_ptr<normal>(new normal(seed))));
		m_cauchy.emplace(make_pair(caller::Problem, std::unique_ptr<cauchy>(new cauchy(seed))));
		m_levy.emplace(make_pair(caller::Problem, std::unique_ptr<levy>(new levy(1.4, seed))));
		m_gamma.emplace(make_pair(caller::Problem, std::unique_ptr<gamma>(new gamma(0.5, seed))));

		if (seed_alg<0 || seed_alg>1) seed = 0.5;
		else seed = seed_alg;

		m_uniform.emplace(make_pair(caller::Algorithm, std::unique_ptr<uniform>(new uniform(seed))));
		m_normal.emplace(make_pair(caller::Algorithm, std::unique_ptr<normal>(new normal(seed))));
		m_cauchy.emplace(make_pair(caller::Algorithm, std::unique_ptr<cauchy>(new cauchy(seed))));
		m_levy.emplace(make_pair(caller::Algorithm, std::unique_ptr<levy>(new levy(1.4, seed))));
		m_gamma.emplace(make_pair(caller::Algorithm, std::unique_ptr<gamma>(new gamma(0.5, seed))));
	}
	global::~global() {
		
	}
	int global::get_rand_int(const int min, const int max, caller mode) {
		if (min == max || min == max - 1) return min;
		if (mode == caller::Algorithm)	return static_cast<int>(min + (max - min)*m_uniform[caller::Algorithm]->next());
		else	return static_cast<int>(min + (max - min)*m_uniform[caller::Problem]->next());
	}
	double global::get_rand_float(const double min, const double max, caller mode) {
		if (min == max) return min;
		if (mode == caller::Algorithm)	return min + (max - min)*m_uniform[caller::Algorithm]->next();
		else		return min + (max - min)*m_uniform[caller::Problem]->next();
	}
}