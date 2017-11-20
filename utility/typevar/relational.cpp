
#include "typevar.h"

namespace OFEC {
	///
	/// typevar == bool
	///
	bool typevar::operator == (bool n) const {
		return *this == typevar(n);
	}

	bool typevar::operator == (char c) const {
		return *this == typevar(c);
	}

	bool typevar::operator == (size_type n) const {
		return *this == typevar(n);
	}
	///
	/// typevar == int
	///
	bool typevar::operator == (int n) const {
		return *this == typevar(n);
	}

	///
	/// typevar == double
	///
	bool typevar::operator == (double n) const {
		return *this == typevar(n);
	}

	///
	/// typevar == string
	///
	bool typevar::operator == (const std::string& s) const {
		return *this == typevar(s);
	}

	///
	/// typevar == string constant
	///
	bool typevar::operator == (const char* s) const {
		return *this == typevar(s);
	}

	///
	/// typevar == wide string
	///
	bool typevar::operator == (const std::wstring& s) const {
		return *this == typevar(s);
	}

	///
	/// typevar == wide string constant
	///
	bool typevar::operator == (const wchar_t* s) const {
		return *this == typevar(s);
	}

	///
	/// typevar == typevar
	///
	template<>
	struct typevar::equal_visitor<bool> 
	{
		using result_type = bool;
		// Different types
		template <typename T, typename U>
		result_type operator () (const T& lhs, const U& rhs) const
		{
			return false;
		}
		// Same types
		template <typename T>
		result_type operator () (const T& lhs, const T& rhs) const
		{
			// Use overloaded functions to handle explicit specialization
			return equal(lhs, rhs);
		}

	private:
		bool equal(const null_t&, const null_t&) const
		{
			return true;
		}
		bool equal(const bool_t& lhs, const bool_t& rhs) const
		{
			return lhs == rhs;
		}
		bool equal(const char_t& lhs, const char_t& rhs) const
		{
			return lhs == rhs;
		}
		bool equal(const int_t& lhs, const int_t& rhs) const
		{
			return lhs == rhs;
		}
		bool equal(const double_t& lhs, const double_t& rhs) const
		{
			return lhs == rhs;
		}
		bool equal(const string_t& lhs, const string_t& rhs) const
		{
			return *(lhs.ps) == *(rhs.ps);
		}
		bool equal(const wstring_t& lhs, const wstring_t& rhs) const
		{
			return *(lhs.ps) == *(rhs.ps);
		}
		bool equal(const size_type& lhs, const size_type& rhs) const
		{
			return lhs == rhs;
		}
	};
	bool typevar::operator == (const typevar& v) const {
		return mapbox::util::apply_visitor(equal_visitor<bool>(), _var, v._var);
	}

	///
	/// typevar != bool
	///
	bool typevar::operator != (bool n) const {
		return *this != typevar(n);
	}

	bool typevar::operator != (char n) const {
		return *this != typevar(n);
	}
	///
	/// typevar != size_type
	///
	bool typevar::operator != (size_type n) const {
		return *this != typevar(n);
	}

	///
	/// typevar != int
	///
	bool typevar::operator != (int n) const {
		return *this != typevar(n);
	}

	///
	/// typevar != double
	///
	bool typevar::operator != (double n) const {
		return *this != typevar(n);
	}

	///
	/// typevar != string
	///
	bool typevar::operator != (const std::string& s) const {
		return *this != typevar(s);
	}

	///
	/// typevar != string constant
	///
	bool typevar::operator != (const char* s) const {
		return *this != typevar(s);
	}

	///
	/// typevar != wide string
	///
	bool typevar::operator != (const std::wstring& s) const {
		return *this != typevar(s);
	}

	///
	/// typevar != wide string constant
	///
	bool typevar::operator != (const wchar_t* s) const {
		return *this != typevar(s);
	}

	///
	/// typevar != typevar
	///
	bool typevar::operator != (const typevar& v) const {
		return !mapbox::util::apply_visitor(equal_visitor<bool>(), _var, v._var);
	}

	///
	/// typevar < bool
	///
	bool typevar::operator < (bool n) const {
		const typevar v(n);
		return less_var()(*this, v);
	}

	bool typevar::operator < (char n) const {
		const typevar v(n);
		return less_var()(*this, v);
	}
	///
	/// typevar < int
	///
	bool typevar::operator < (int n) const {
		const typevar v(n);
		return less_var()(*this, v);
	}

	///
	/// typevar < size_t
	///
	bool typevar::operator < (size_type n) const {
		const typevar v(n);
		return less_var()(*this, v);
	}

	///
	/// typevar < double
	///
	bool typevar::operator < (double n) const {
		const typevar v(n);
		return less_var()(*this, v);
	}

	///
	/// typevar < string
	///
	bool typevar::operator < (const std::string& s) const {
		const typevar v(s);
		return less_var()(*this, v);
	}

	///
	/// typevar < string constant
	///
	bool typevar::operator < (const char* s) const {
		const typevar v(s);
		return less_var()(*this, v);
	}

	///
	/// typevar < wide string
	///
	bool typevar::operator < (const std::wstring& s) const {
		const typevar v(s);
		return less_var()(*this, v);
	}

	///
	/// typevar < wide string constant
	///
	bool typevar::operator < (const wchar_t* s) const {
		const typevar v(s);
		return less_var()(*this, v);
	}

	///
	/// typevar < typevar
	///
	bool typevar::operator < (const typevar& v) const {
		return less_var()(*this, v);
	}

	///
	/// typevar <= bool
	///
	bool typevar::operator <= (bool n) const {
		if (is_bool()) return mapbox::util::get<bool_t>(_var) <= n;
		THROW("invalid <= comparison to bool");
	}

	bool typevar::operator <= (char n) const {
		if (is_char()) return mapbox::util::get<char_t>(_var) <= n;
		THROW("invalid <= comparison to char");
	}

	///
	/// typevar <= int
	///
	bool typevar::operator <= (int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) <= n;
		THROW("invalid <= comparison to int");
	}

	///
	/// typevar <= double
	///
	bool typevar::operator <= (double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) <= n;
		THROW("invalid <= comparison to double");
	}

	///
	/// typevar <= string
	///
	bool typevar::operator <= (const std::string& s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps <= s;
		THROW("invalid <= comparison to string");
	}

	///
	/// typevar <= string constant
	///
	bool typevar::operator <= (const char* s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps <= s;
		THROW("invalid <= comparison to char*");
	}

	///
	/// typevar <= wide string
	///
	bool typevar::operator <= (const std::wstring& s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps <= s;
		THROW("invalid <= comparison to wstring");
	}

	///
	/// typevar <= wide string constant
	///
	bool typevar::operator <= (const wchar_t* s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps <= s;
		THROW("invalid <= comparison to wchar_t*");
	}

	///
	/// typevar <= typevar
	///
	bool typevar::operator <= (const typevar& v) const {
		switch (type()) {
		case type_null:    THROW("invalid <= comparison to none");
		case type_char:		return v.is_char() && mapbox::util::get<char_t>(_var) <= mapbox::util::get<char_t>(v._var);
		case type_bool:    return v.is_bool() && mapbox::util::get<bool_t>(_var) <= mapbox::util::get<bool_t>(v._var);
		case type_int:     return v.is_int() && mapbox::util::get<int_t>(_var) <= mapbox::util::get<int_t>(v._var);
		case type_double:  return v.is_double() && mapbox::util::get<double_t>(_var) <= mapbox::util::get<double_t>(v._var);
		case type_string:  return v.is_string() && *mapbox::util::get<string_t>(_var).ps <= *mapbox::util::get<string_t>(v._var).ps;
		case type_wstring: return v.is_wstring() && *mapbox::util::get<wstring_t>(_var).ps <= *mapbox::util::get<wstring_t>(v._var).ps;
		case type_size_t:   return v.is_size_t() && mapbox::util::get<size_type>(_var) <= mapbox::util::get<size_type>(v._var);
		default:           THROW("(unhandled type) <= not implemented");
		}
	}

	///
	/// typevar > bool
	///
	bool typevar::operator > (bool n) const {
		if (is_bool()) return mapbox::util::get<bool_t>(_var) > n;
		THROW("invalid > comparison to bool");
	}

	bool typevar::operator > (char n) const {
		if (is_char()) return mapbox::util::get<char_t>(_var) > n;
		THROW("invalid > comparison to bool");
	}
	///
	/// typevar > int
	///
	bool typevar::operator > (int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) > n;
		THROW("invalid > comparison to int");
	}

	///
	/// typevar > double
	///
	bool typevar::operator > (double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) > n;
		THROW("invalid > comparison to double");
	}

	///
	/// typevar > string
	///
	bool typevar::operator > (const std::string& s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps > s;
		THROW("invalid > comparison to string");
	}

	///
	/// typevar > string constant
	bool typevar::operator > (const char* s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps > s;
		THROW("invalid > comparison to char*");
	}

	///
	/// typevar > wide string
	///
	bool typevar::operator > (const std::wstring& s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps > s;
		THROW("invalid > comparison to wstring");
	}

	///
	/// typevar > wide string constant
	///
	bool typevar::operator > (const wchar_t* s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps > s;
		THROW("invalid > comparison to wchar_t*");
	}

	///
	/// typevar > typevar
	///
	bool typevar::operator > (const typevar& v) const {
		switch (type()) {
		case type_null:    THROW("invalid > comparison to none");
		case type_char:    return v.is_char() && mapbox::util::get<char_t>(_var) > mapbox::util::get<char_t>(v._var);
		case type_bool:    return v.is_bool() && mapbox::util::get<bool_t>(_var) > mapbox::util::get<bool_t>(v._var);
		case type_int:     return v.is_int() && mapbox::util::get<int_t>(_var) > mapbox::util::get<int_t>(v._var);
		case type_double:  return v.is_double() && mapbox::util::get<double_t>(_var) > mapbox::util::get<double_t>(v._var);
		case type_string:  return v.is_string() && *mapbox::util::get<string_t>(_var).ps > *mapbox::util::get<string_t>(v._var).ps;
		case type_wstring: return v.is_wstring() && *mapbox::util::get<wstring_t>(_var).ps > *mapbox::util::get<wstring_t>(v._var).ps;
		case type_size_t:   return v.is_size_t() && mapbox::util::get<size_type>(_var) > mapbox::util::get<size_type>(v._var);
		default:           THROW("(unhandled type) > not implemented");
		}
	}

	///
	/// typevar >= bool
	///
	bool typevar::operator >= (bool n) const {
		if (is_bool()) return mapbox::util::get<bool_t>(_var) >= n;
		THROW("invalid >= comparison to bool");
	}

	bool typevar::operator >= (char n) const {
		if (is_char()) return mapbox::util::get<char_t>(_var) >= n;
		THROW("invalid >= comparison to char");
	}


	///
	/// typevar >= int
	///
	bool typevar::operator >= (int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) >= n;
		THROW("invalid >= comparison to int");
	}

	///
	/// typevar >= double
	///
	bool typevar::operator >= (double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) >= n;
		THROW("invalid >= comparison to double");
	}

	///
	/// typevar >= string
	///
	bool typevar::operator >= (const std::string& s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps >= s;
		THROW("invalid >= comparison to string");
	}

	///
	/// typevar >= string constant
	///
	bool typevar::operator >= (const char* s) const {
		if (is_string()) return *mapbox::util::get<string_t>(_var).ps >= s;
		THROW("invalid >= comparison to char*");
	}

	///
	/// typevar >= wide string
	///
	bool typevar::operator >= (const std::wstring& s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps >= s;
		THROW("invalid >= comparison to wstring");
	}

	///
	/// typevar >= wide string constant
	///
	bool typevar::operator >= (const wchar_t* s) const {
		if (is_wstring()) return *mapbox::util::get<wstring_t>(_var).ps >= s;
		THROW("invalid >= comparison to wchar_t*");
	}

	///
	/// typevar >= typevar
	///
	bool typevar::operator >= (const typevar& v) const {
		switch (type()) {
		case type_null:    THROW("invalid >= comparison to none");
		case type_bool:    return v.is_bool() && mapbox::util::get<bool_t>(_var) >= mapbox::util::get<bool_t>(v._var);
		case type_char:     return v.is_int() && mapbox::util::get<char_t>(_var) >= mapbox::util::get<char_t>(v._var);
		case type_int:     return v.is_int() && mapbox::util::get<int_t>(_var) >= mapbox::util::get<int_t>(v._var);
		case type_double:  return v.is_double() && mapbox::util::get<double_t>(_var) >= mapbox::util::get<double_t>(v._var);
		case type_string:  return v.is_string() && *mapbox::util::get<string_t>(_var).ps >= *mapbox::util::get<string_t>(v._var).ps;
		case type_wstring: return v.is_wstring() && *mapbox::util::get<wstring_t>(_var).ps >= *mapbox::util::get<wstring_t>(v._var).ps;
		case type_size_t:   return v.is_size_t() && mapbox::util::get<size_type>(_var) >= mapbox::util::get<size_type>(v._var);
		default:           THROW("(unhandled type) >= not implemented");
		}
	}

	bool typevar::operator >= (size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) >= n;
		THROW("invalid >= comparison to size_t");
	}

	bool typevar::operator <= (size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) <= n;
		THROW("invalid <= comparison to size_t");
	}
	bool typevar::operator > (size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) > n;
		THROW("invalid > comparison to size_t");
	}

}