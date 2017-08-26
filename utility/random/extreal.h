#ifndef EXTREAL_LIB_H
#define EXTREAL_LIB_H 0

#include <iostream>
#include "../../core/definition.h"

namespace OFEC {

/************************ extended real class ***************************/

enum class EXT_REAL_CODE
   { Finite, PlusInfinity, MinusInfinity, Indefinite, Missing };

class ext_real
{
   real m_value;
   EXT_REAL_CODE m_c;
public:
   ext_real operator+(const ext_real&) const;
   ext_real operator-(const ext_real&) const;
   ext_real operator*(const ext_real&) const;
   ext_real operator-() const;
   friend std::ostream& operator<<( std::ostream&, const ext_real& );
   ext_real(real v): m_value(v),m_c(EXT_REAL_CODE::Finite){   }
   ext_real(const EXT_REAL_CODE& cx):m_c(cx) { }
   ext_real():m_c(EXT_REAL_CODE::Missing) {  }
   real value() const { return m_value; }
   bool is_real() const { return m_c== EXT_REAL_CODE::Finite; }
   EXT_REAL_CODE code() const { return m_c; }
};


}

#endif

// body file: extreal.cpp



