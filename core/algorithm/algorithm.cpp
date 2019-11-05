
#include "../global.h"

namespace OFEC {

	algorithm::algorithm(const std::string name) :m_name(name) {
		//set TermMaxFes as default stop criteria
		if (global::ms_arg.find("maxEvals") != global::ms_arg.end()) {
			m_term.reset(new term_max_evals(global::ms_arg));
		}
		else {
			m_term.reset(new termination());
		}
	}
	void algorithm::run(){
		run_();
		m_term->set_terminated_true();		
	}

	bool algorithm::terminated() {
		return m_term->terminated();
	}
	bool algorithm::terminating() {
		if (auto t = dynamic_cast<term_max_evals*>(m_term.get())) //by default 
			return t->terminating(global::ms_global->m_problem->evaluations());
		else
			return true;
	}
	real algorithm::duration() {
		return m_term->duration();
	}
	void algorithm::set_termination(termination* t) {
		m_term.reset(t);
	}

	void algorithm::update_parameters() {
		m_parameters["name"] = m_name;
		m_parameters["stop criterion"] = m_term->criterion();

	}
}
