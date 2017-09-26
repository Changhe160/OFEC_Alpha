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
		m_uniform[caller::Problem].reset();
		m_normal[caller::Problem].reset();
		m_cauchy[caller::Problem].reset();
		m_levy[caller::Problem].reset();
		m_gamma[caller::Problem].reset();
		m_uniform[caller::Algorithm].reset();
		m_normal[caller::Algorithm].reset();
		m_cauchy[caller::Algorithm].reset();
		m_levy[caller::Algorithm].reset();
		m_gamma[caller::Algorithm].reset();
	}
}