#include "instance.h"

namespace ofec {
	void Instance::recordInputParameters() {
		m_input_parameters.checkValue();
		m_input_parameters.output(m_archived_parameters);
	}

	void Instance::restoreInputParameters() {
		m_input_parameters.input(m_archived_parameters);
	}
}
