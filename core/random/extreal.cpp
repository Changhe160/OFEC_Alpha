
#include "extreal.h"

namespace ofec {

ExtReal ExtReal::operator+(const ExtReal& er) const
{
   if (m_c==ExtRealCode::Finite && er.m_c==ExtRealCode::Finite) return ExtReal(m_value+er.m_value);
   if (m_c==ExtRealCode::Missing || er.m_c==ExtRealCode::Missing) return ExtReal(ExtRealCode::Missing);
   if (m_c==ExtRealCode::Indefinite || er.m_c==ExtRealCode::Indefinite) return ExtReal(ExtRealCode::Indefinite);
   if (m_c==ExtRealCode::PlusInfinity)
   {
      if (er.m_c==ExtRealCode::MinusInfinity) return ExtReal(ExtRealCode::Indefinite);
      return *this;
   }
   if (m_c==ExtRealCode::MinusInfinity)
   {
      if (er.m_c==ExtRealCode::PlusInfinity) return ExtReal(ExtRealCode::Indefinite);
      return *this;
   }
   return er;
}

ExtReal ExtReal::operator-(const ExtReal& er) const
{
   if (m_c==ExtRealCode::Finite && er.m_c==ExtRealCode::Finite) return ExtReal(m_value-er.m_value);
   if (m_c==ExtRealCode::Missing || er.m_c==ExtRealCode::Missing) return ExtReal(ExtRealCode::Missing);
   if (m_c==ExtRealCode::Indefinite || er.m_c==ExtRealCode::Indefinite) return ExtReal(ExtRealCode::Indefinite);
   if (m_c==ExtRealCode::PlusInfinity)
   {
      if (er.m_c==ExtRealCode::PlusInfinity) return ExtReal(ExtRealCode::Indefinite);
      return *this;
   }
   if (m_c==ExtRealCode::MinusInfinity)
   {
      if (er.m_c==ExtRealCode::MinusInfinity) return ExtReal(ExtRealCode::Indefinite);
      return *this;
   }
   return er;
}

ExtReal ExtReal::operator*(const ExtReal& er) const
{
   if (m_c==ExtRealCode::Finite && er.m_c==ExtRealCode::Finite) return ExtReal(m_value*er.m_value);
   if (m_c==ExtRealCode::Missing || er.m_c==ExtRealCode::Missing) return ExtReal(ExtRealCode::Missing);
   if (m_c==ExtRealCode::Indefinite || er.m_c==ExtRealCode::Indefinite) return ExtReal(ExtRealCode::Indefinite);
   if (m_c==ExtRealCode::Finite)
   {
      if (m_value==0.0) return ExtReal(ExtRealCode::Indefinite);
      if (m_value>0.0) return er;
      return (-er);
   }
   if (er.m_c==ExtRealCode::Finite)
   {
      if (er.m_value==0.0) return ExtReal(ExtRealCode::Indefinite);
      if (er.m_value>0.0) return *this;
      return -(*this);
   }
   if (m_c==ExtRealCode::PlusInfinity) return er;
   return (-er);
}

ExtReal ExtReal::operator-() const
{
   switch (m_c)
   {
      case ExtRealCode::Finite:        return ExtReal(-m_value);
      case ExtRealCode::PlusInfinity:  return ExtReal(ExtRealCode::MinusInfinity);
      case ExtRealCode::MinusInfinity: return ExtReal(ExtRealCode::PlusInfinity);
      case ExtRealCode::Indefinite:    return ExtReal(ExtRealCode::Indefinite);
      case ExtRealCode::Missing:       return ExtReal(ExtRealCode::Missing);
   }
   return 0.0;
}

std::ostream& operator<<(std::ostream& os, const ExtReal& er)
{
   switch (er.m_c)
   {
      case ExtRealCode::Finite:        os << er.m_value;         break;
      case ExtRealCode::PlusInfinity:  os << "plus-infinity";  break;
      case ExtRealCode::MinusInfinity: os << "minus-infinity"; break;
      case ExtRealCode::Indefinite:    os << "indefinite";     break;
      case ExtRealCode::Missing:       os << "missing";        break;
   }
   return os;
}

}


