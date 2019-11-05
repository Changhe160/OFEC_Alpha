#include "global.h"

namespace OFEC {
	
	thread_local std::shared_ptr<global> global::ms_global = nullptr;

#ifdef USING_GPU
	compute::device global::m_device = compute::system::default_device();
#endif // USING_GPU

	param_map global::ms_arg;
	std::map<const std::string, bool> global::ms_filename_info;
	std::map<const std::string, const std::string> global::ms_param;
	factory<problem> global::ms_reg_problem;
	factory<algorithm> global::ms_reg_algorithm;
	size_t global::ms_sample_fre = 1;
	std::map<std::string, std::set<std::string>> global::ms_alg4pro;

	global::global(const int runID, real seed_pro, real seed_alg) :  m_runID(runID){
		double seed[3] = { seed_pro ,seed_alg ,0.5 };
		for (size_t i = static_cast<size_t>(caller::Begin); i != static_cast<size_t>(caller::End); ++i) {
			auto call = static_cast<caller>(i);
			m_uniform.emplace(std::make_pair(call, std::unique_ptr<uniform>(new uniform(seed[i]))));
			m_normal.emplace(std::make_pair(call, std::unique_ptr<normal>(new normal(seed[i]))));
			m_cauchy.emplace(std::make_pair(call, std::unique_ptr<cauchy>(new cauchy(seed[i]))));
			m_levy.emplace(std::make_pair(call, std::unique_ptr<levy>(new levy(1.4, seed[i]))));
			m_gamma.emplace(std::make_pair(call, std::unique_ptr<gamma>(new gamma(0.5, seed[i]))));

		}
	}


}
