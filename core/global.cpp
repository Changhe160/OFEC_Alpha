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
}