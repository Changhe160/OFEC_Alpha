#ifndef OFEC_EPA_ENCODING_H
#define OFEC_EPA_ENCODING_H

#include <vector>
#include <functional>

#include "../../../../utility/myexcept.h"
#include "../../../../utility/functional.h"
#include "../../../../utility/typevar/typevar.h"


namespace OFEC {
	class variable_epanet {
	public:
		variable_epanet() {};
		variable_epanet(const variable_epanet& rhs) : m_x(rhs.m_x) {}
		variable_epanet(variable_epanet&& rhs) :m_x(std::move(rhs.m_x)) {}

		variable_epanet& operator=(const variable_epanet& rhs) {
			if (this == &rhs) return *this;
			m_x = rhs.m_x;
			return *this;
		}

		variable_epanet& operator=(variable_epanet&& rhs) {
			m_x = std::move(rhs.m_x);
			return *this;
		}

		char* location() {
			return m_x.Loc;
		}

		long & duration() {
			return m_x.Duration;
		}

		float & source() {
			return m_x.Source;
		}

		long & start_time() {
			return m_x.startTime;
		}

		std::vector<float> & multiplier() {
			return m_x.Multiplier;
		}

		bool & flag_location() {
			return m_x.isLoc;
		}

		int & index() {
			return m_x.Index;
		}
		bool & is_detected() {
			return m_x.isDetected;
		}
		int & interval() {
			return m_x.Interval;
		}
	protected:
		struct epa_type : typevar {
			int Interval = 0;
			bool isLoc = true;
			bool isDetected = false;
			char Loc[32];
			int Index;
			float Source;
			long startTime;
			long Duration;
			std::vector<float> Multiplier;
		};
		epa_type m_x;
	};
}

#endif //!OFEC_EPA_ENCODING_H
