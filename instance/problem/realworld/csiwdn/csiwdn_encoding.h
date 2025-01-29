#ifndef OFEC_EPA_ENCODING_H
#define OFEC_EPA_ENCODING_H

#include <vector>
#include <functional>

#include "../../../../core/exception.h"
#include "../../../../utility/functional.h"
#include "../../../../core/problem/encoding.h"

namespace ofec {
	class VarCSIWDN :public VariableBase{
	protected:
		struct epa_type {
			int Interval = 0;
			char Loc[32];
			std::string sLoc;
			int Index;
			float Source = 1.0;
			long startTime;
			long Duration;
			std::vector<Real> Multiplier;
		};

		std::vector<epa_type> m_variables;
		bool isLoc = true;
		bool isDet = false;
		long firstDetTime;

	public:
		VarCSIWDN() {};

		VarCSIWDN(size_t size) :m_variables(size) {}
		VarCSIWDN(const VarCSIWDN& rhs) : m_variables(rhs.m_variables),isLoc(rhs.isLoc),isDet(rhs.isDet),firstDetTime(rhs.firstDetTime) {}
		VarCSIWDN(VarCSIWDN&& rhs) :m_variables(std::move(rhs.m_variables)), isLoc(std::move(rhs.isLoc)), isDet(std::move(rhs.isDet)), firstDetTime(std::move(rhs.firstDetTime)) {}

		VarCSIWDN& operator=(const VarCSIWDN& rhs) {
			if (this == &rhs) return *this;
			m_variables = rhs.m_variables;
			isLoc = rhs.isLoc;
			isDet = rhs.isDet;
			firstDetTime = rhs.firstDetTime;
			return *this;
		}
		virtual VariableBase& operator=(const VariableBase& rhs) override {
			auto cursol = dynamic_cast<const VarCSIWDN*>(&rhs);
			if (cursol != nullptr) {
				m_variables = cursol->m_variables;
				isLoc = cursol->isLoc;
				isDet = cursol->isDet;
				firstDetTime = cursol->firstDetTime;
			}
			return *this;
		}

		VarCSIWDN& operator=(VarCSIWDN&& rhs) {
			m_variables = std::move(rhs.m_variables);
			isLoc = std::move(rhs.isLoc);
			isDet = std::move(rhs.isDet);
			firstDetTime = std::move(rhs.firstDetTime);
			return *this;
		}

		void copy(const VariableBase& x)  {
			auto& rhs = dynamic_cast<const VarCSIWDN&>(x);
			this->operator=(rhs);
		}

		size_t numberSource() const {
			return m_variables.size();
		}

		void append(const epa_type x) {
			m_variables.push_back(x);
		}

		void resizeEpaSet(size_t n) {
			m_variables.resize(n);
		}

		epa_type& getEpa(size_t i) {
			return m_variables[i];
		}

		char* location(size_t i) {
			return m_variables[i].Loc;
		}

		std::string& slocation(size_t i) {
			return m_variables[i].sLoc;
		}

		const std::string &slocation(size_t i) const {
			return m_variables[i].sLoc;
		}

		long& duration(size_t i) {
			return m_variables[i].Duration;
		}

		long duration(size_t i) const {
			return m_variables[i].Duration;
		}

		float& source(size_t i) {
			return m_variables[i].Source;
		}

		long& startTime(size_t i) {
			return m_variables[i].startTime;
		}

		long startTime(size_t i) const {
			return m_variables[i].startTime;
		}

		std::vector<Real>& multiplier(size_t i) {
			return m_variables[i].Multiplier;
		}

		const std::vector<Real>& multiplier(size_t i) const {
			return m_variables[i].Multiplier;
		}

		bool& flagLocation() {
			return isLoc;
		}
		int index(size_t i)const {
			return m_variables[i].Index;
		}
		int& index(size_t i) {
			return m_variables[i].Index;
		}
		bool &isDetected() {
			return isDet;
		}
		long &firstDetectedTime() {
			return firstDetTime;
		}
		int& interval(size_t i) {
			return m_variables[i].Interval;
		}
		size_t size() const noexcept {
			return 1;
		}
		void resize(size_t n) {
			m_variables.resize(n);
		}
	};
}

#endif //!OFEC_EPA_ENCODING_H
