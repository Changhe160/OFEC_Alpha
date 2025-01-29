#include "optima.h"

namespace ofec {
    OptimaBase& OptimaBase::operator=(const OptimaBase &rhs) {
        if (this != &rhs) {
            m_objectives = rhs.m_objectives;
        }
        return *this;
    }
}