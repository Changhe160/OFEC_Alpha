
#include "extreal.h"

namespace OFEC {

ext_real ext_real::operator+(const ext_real& er) const
{
   if (m_c==EXT_REAL_CODE::Finite && er.m_c==EXT_REAL_CODE::Finite) return ext_real(m_value+er.m_value);
   if (m_c==EXT_REAL_CODE::Missing || er.m_c==EXT_REAL_CODE::Missing) return ext_real(EXT_REAL_CODE::Missing);
   if (m_c==EXT_REAL_CODE::Indefinite || er.m_c==EXT_REAL_CODE::Indefinite) return ext_real(EXT_REAL_CODE::Indefinite);
   if (m_c==EXT_REAL_CODE::PlusInfinity)
   {
      if (er.m_c==EXT_REAL_CODE::MinusInfinity) return ext_real(EXT_REAL_CODE::Indefinite);
      return *this;
   }
   if (m_c==EXT_REAL_CODE::MinusInfinity)
   {
      if (er.m_c==EXT_REAL_CODE::PlusInfinity) return ext_real(EXT_REAL_CODE::Indefinite);
      return *this;
   }
   return er;
}

ext_real ext_real::operator-(const ext_real& er) const
{
   if (m_c==EXT_REAL_CODE::Finite && er.m_c==EXT_REAL_CODE::Finite) return ext_real(m_value-er.m_value);
   if (m_c==EXT_REAL_CODE::Missing || er.m_c==EXT_REAL_CODE::Missing) return ext_real(EXT_REAL_CODE::Missing);
   if (m_c==EXT_REAL_CODE::Indefinite || er.m_c==EXT_REAL_CODE::Indefinite) return ext_real(EXT_REAL_CODE::Indefinite);
   if (m_c==EXT_REAL_CODE::PlusInfinity)
   {
      if (er.m_c==EXT_REAL_CODE::PlusInfinity) return ext_real(EXT_REAL_CODE::Indefinite);
      return *this;
   }
   if (m_c==EXT_REAL_CODE::MinusInfinity)
   {
      if (er.m_c==EXT_REAL_CODE::MinusInfinity) return ext_real(EXT_REAL_CODE::Indefinite);
      return *this;
   }
   return er;
}

ext_real ext_real::operator*(const ext_real& er) const
{
   if (m_c==EXT_REAL_CODE::Finite && er.m_c==EXT_REAL_CODE::Finite) return ext_real(m_value*er.m_value);
   if (m_c==EXT_REAL_CODE::Missing || er.m_c==EXT_REAL_CODE::Missing) return ext_real(EXT_REAL_CODE::Missing);
   if (m_c==EXT_REAL_CODE::Indefinite || er.m_c==EXT_REAL_CODE::Indefinite) return ext_real(EXT_REAL_CODE::Indefinite);
   if (m_c==EXT_REAL_CODE::Finite)
   {
      if (m_value==0.0) return ext_real(EXT_REAL_CODE::Indefinite);
      if (m_value>0.0) return er;
      return (-er);
   }
   if (er.m_c==EXT_REAL_CODE::Finite)
   {
      if (er.m_value==0.0) return ext_real(EXT_REAL_CODE::Indefinite);
      if (er.m_value>0.0) return *this;
      return -(*this);
   }
   if (m_c==EXT_REAL_CODE::PlusInfinity) return er;
   return (-er);
}

ext_real ext_real::operator-() const
{
   switch (m_c)
   {
      case EXT_REAL_CODE::Finite:        return ext_real(-m_value);
      case EXT_REAL_CODE::PlusInfinity:  return ext_real(EXT_REAL_CODE::MinusInfinity);
      case EXT_REAL_CODE::MinusInfinity: return ext_real(EXT_REAL_CODE::PlusInfinity);
      case EXT_REAL_CODE::Indefinite:    return ext_real(EXT_REAL_CODE::Indefinite);
      case EXT_REAL_CODE::Missing:       return ext_real(EXT_REAL_CODE::Missing);
   }
   return 0.0;
}

std::ostream& operator<<(std::ostream& os, const ext_real& er)
{
   switch (er.m_c)
   {
      case EXT_REAL_CODE::Finite:        os << er.m_value;         break;
      case EXT_REAL_CODE::PlusInfinity:  os << "plus-infinity";  break;
      case EXT_REAL_CODE::MinusInfinity: os << "minus-infinity"; break;
      case EXT_REAL_CODE::Indefinite:    os << "indefinite";     break;
      case EXT_REAL_CODE::Missing:       os << "missing";        break;
   }
   return os;
}

}


