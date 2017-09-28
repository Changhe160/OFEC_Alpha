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

	global::global(double seed_pro, double seed_alg) {
		
		m_uniform.emplace(make_pair(caller::Problem, std::unique_ptr<uniform>(new uniform(seed_pro))));
		m_normal.emplace(make_pair(caller::Problem, std::unique_ptr<normal>(new normal(seed_pro))));
		m_cauchy.emplace(make_pair(caller::Problem, std::unique_ptr<cauchy>(new cauchy(seed_pro))));
		m_levy.emplace(make_pair(caller::Problem, std::unique_ptr<levy>(new levy(1.4, seed_pro))));
		m_gamma.emplace(make_pair(caller::Problem, std::unique_ptr<gamma>(new gamma(0.5, seed_pro))));

		m_uniform.emplace(make_pair(caller::Algorithm, std::unique_ptr<uniform>(new uniform(seed_alg))));
		m_normal.emplace(make_pair(caller::Algorithm, std::unique_ptr<normal>(new normal(seed_alg))));
		m_cauchy.emplace(make_pair(caller::Algorithm, std::unique_ptr<cauchy>(new cauchy(seed_alg))));
		m_levy.emplace(make_pair(caller::Algorithm, std::unique_ptr<levy>(new levy(1.4, seed_alg))));
		m_gamma.emplace(make_pair(caller::Algorithm, std::unique_ptr<gamma>(new gamma(0.5, seed_alg))));

	}
}