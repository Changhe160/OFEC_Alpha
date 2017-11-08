
#include "../global.h"

namespace OFEC {

	algorithm::algorithm(const std::string name) :m_name(name) {
		//set TermMaxFes as default stop criteria
#if defined OFEC_DEMON
		m_term.reset(new termination(global::ms_arg));
#else
		if (global::ms_arg.find(param_maxEvals) != global::ms_arg.end()) {
			m_term.reset(new term_max_evals(global::ms_arg));
		}
		else {
			m_term.reset(new termination());
		}
#endif
		m_parameter << "Algorithm: " << m_name << std::endl;
	}
	evaluation_tag algorithm::run(){
		evaluation_tag tag = run_();
		m_term->set_terminated_true();
		return tag;
	}

	bool algorithm::terminated() {
		return m_term->terminated();
	}
	bool algorithm::terminating() {
		return m_term->terminating();
	}
	double algorithm::duration() {
		return m_term->duration();
	}
	void algorithm::set_termination(termination* t) {
		m_term.reset(t);
	}

}
