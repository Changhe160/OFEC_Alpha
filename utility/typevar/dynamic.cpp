
#include<iomanip>
#include <cctype>
#include <cwctype>
#include "typevar.h"

namespace OFEC {

	namespace typevar_None {
		const typevar typeNone;
	};
	bool typevar::less_var::operator () (const typevar& lhs, const typevar& rhs) {
		// if the two vars are of different types, order by type
		code lht = lhs.type(), rht = rhs.type();
		if (lht != rht) return lht < rht;

		// they are of the same type, order by value
		switch (lht) {
		case type_null: return false;
		case type_char: return mapbox::util::get<char_t>(lhs._var) < mapbox::util::get<char_t>(rhs._var);
		case type_bool: return mapbox::util::get<bool_t>(lhs._var) < mapbox::util::get<bool_t>(rhs._var);
		case type_int: return mapbox::util::get<int_t>(lhs._var) < mapbox::util::get<int_t>(rhs._var);
		case type_double: return mapbox::util::get<double_t>(lhs._var) < mapbox::util::get<double_t>(rhs._var);
		case type_string: return *(mapbox::util::get<string_t>(lhs._var).ps) < *(mapbox::util::get<string_t>(rhs._var).ps);
		case type_wstring: return *(mapbox::util::get<wstring_t>(lhs._var).ps) < *(mapbox::util::get<wstring_t>(rhs._var).ps);
		default: THROW("unhandled type");
		}
	}
	/*
	///
	/// append a bool to a collection
	///
	typevar& typevar::operator () (bool n) { return operator() (typevar(n)); }

	typevar& typevar::operator () (char n) { return operator() (typevar(n)); }

	///
	/// append an int to a collection
	///
	typevar& typevar::operator () (int n) { return operator() (typevar(n)); }

	///
	/// append a double to a collection
	///
	typevar& typevar::operator () (double n) { return operator() (typevar(n)); }

	///
	/// append a string to a collection
	///
	typevar& typevar::operator () (const std::string& s) { return operator() (typevar(s)); }

	///
	/// append a string constant to a collection
	///
	typevar& typevar::operator () (const char* s) { return operator() (typevar(s)); }

	///
	/// append a wide string to a collection
	///
	typevar& typevar::operator () (const std::wstring& s) { return operator() (typevar(s)); }

	///
	/// append a wide string constant to a collection
	///
	typevar& typevar::operator () (const wchar_t* s) { return operator() (typevar(s)); }

	*/
	///
	/// count of objects in a collection or characters in a string
	///
	template<>
	struct typevar::count_visitor<typevar::size_type> 
	{
		using result_type = typevar::size_type;
		result_type operator () (const null_t&) const { return 0; }
		result_type operator () (const char_t&) const { return 1; }
		result_type operator () (const bool_t&) const { return 1; }
		result_type operator () (const int_t&) const { return 1; }
		result_type operator () (const double_t&) const { return 1; }
		result_type operator () (const size_type&) const { return 1; }
		result_type operator () (const string_t& value) const { return value.ps->length(); }
		result_type operator () (const wstring_t& value) const { return value.ps->length(); }
	};
	typevar::size_type typevar::count() const {
		return mapbox::util::apply_visitor(count_visitor<typevar::size_type>(), _var);
	}
	/*
	///
	/// index a collection with a double
	///
	typevar& typevar::operator [] (double n) { return operator[] (typevar(n)); }

	///
	/// index a collection with a string
	///
	typevar& typevar::operator [] (const std::string& s) { return operator[] (typevar(s)); }

	///
	/// index a collection with a string constant
	///
	typevar& typevar::operator [] (const char* s) { return operator[] (typevar(s)); }

	///
	/// index a collection with a wide string
	///
	typevar& typevar::operator [] (const std::wstring& s) { return operator[] (typevar(s)); }

	///
	/// index a collection with a wide string constant
	///
	typevar& typevar::operator [] (const wchar_t* s) { return operator[] (typevar(s)); }


		*/
		///
		/// write a typevar to an ostream
		///
	std::ostream& typevar::_write_var(std::ostream& os) const {
		switch (type()) {
		case type_null:    os << "null"; return os;
		case type_bool:     os << (mapbox::util::get<bool_t>(_var) ? "true" : "false"); return os;
		case type_char:     os << mapbox::util::get<char_t>(_var); return os;
		case type_int:     os << mapbox::util::get<int_t>(_var); return os;
		case type_double:  os << mapbox::util::get<double_t>(_var); return os;
		case type_string:  return _write_string(os);
		case type_wstring: return _write_wstring(os);
		default:           THROW("typevar::_write_var(ostream) unhandled type");
		}
	}

	///
	/// write a string to an ostream
	///
	std::ostream& typevar::_write_string(std::ostream& os) const {
		assert(is_string());
		//os << '"';
		for (const char* s = (*mapbox::util::get<string_t>(_var).ps).c_str(); *s; ++s)
			switch (*s) {
			case '\b': os << "\\b"; break;
			case '\r': os << "\\r"; break;
			case '\n': os << "\\n"; break;
			case '\f': os << "\\f"; break;
			case '\t': os << "\\t"; break;
			case '\\': os << "\\\\"; break;
			case '\"': os << "\\\""; break;
				//case '/' : os << "\\/"; break;
			default:
				if (std::iscntrl(*s)) os << "0" << std::oct << std::setw(3) << std::setfill('0') << int(*s);
				else os << *s;
			}
		//os << '"';
		return os;
	}

	///
	/// write a wide string to an ostream
	///
	std::ostream& typevar::_write_wstring(std::ostream& os) const {
		assert(is_wstring());
		//os << '"';
		for (const wchar_t* s = (*mapbox::util::get<wstring_t>(_var).ps).c_str(); *s; ++s)
			switch (*s) {
			case L'\b': os << L"\\b"; break;
			case L'\r': os << L"\\r"; break;
			case L'\n': os << L"\\n"; break;
			case L'\f': os << L"\\f"; break;
			case L'\t': os << L"\\t"; break;
			case L'\\': os << L"\\\\"; break;
			case L'\"': os << L"\\\""; break;
				//case L'/' : os << L"\\/"; break;
			default:
				if (std::iswcntrl(*s)) os << L"0" << std::oct << std::setw(3) << std::setfill('0') << int(*s);
				else os << *s;
			}
		//os << '"';
		return os;
	}


	///
	/// write a typevar to a wostream
	///
	std::wostream& typevar::_write_var(std::wostream& os) const {
		switch (type()) {
		case type_null:    os << "null"; return os;
		case type_bool:     os << (mapbox::util::get<bool_t>(_var) ? "true" : "false"); return os;
		case type_char:     os << mapbox::util::get<char_t>(_var); return os;
		case type_int:     os << mapbox::util::get<int_t>(_var); return os;
		case type_double:  os << mapbox::util::get<double_t>(_var); return os;
		case type_string:  return _write_string(os);
		case type_wstring: return _write_wstring(os);
		default:           THROW("typevar::_write_var(wostream) unhandled type");
		}
	}

	///
	/// write a string to a wostream
	///
	std::wostream& typevar::_write_string(std::wostream& os) const {
		assert(is_string());
		os << '\'';
		for (const char* s = (*mapbox::util::get<string_t>(_var).ps).c_str(); *s; ++s)
			switch (*s) {
			case '\b': os << "\\b"; break;
			case '\r': os << "\\r"; break;
			case '\n': os << "\\n"; break;
			case '\f': os << "\\f"; break;
			case '\t': os << "\\t"; break;
			case '\\': os << "\\\\"; break;
			case '\'': os << "\\'"; break;
			default:
				if (*s < ' ') os << "0" << std::oct << std::setw(3) << std::setfill(L'0') << int(*s);
				else os << *s;
			}
		os << '\'';
		return os;
	}

	///
	/// write a wide string to a wostream
	///
	std::wostream& typevar::_write_wstring(std::wostream& os) const {
		assert(is_wstring());
		os << '\'';
		for (const wchar_t* s = (*mapbox::util::get<wstring_t>(_var).ps).c_str(); *s; ++s)
			switch (*s) {
			case '\b': os << L"\\b"; break;
			case '\r': os << L"\\r"; break;
			case '\n': os << L"\\n"; break;
			case '\f': os << L"\\f"; break;
			case '\t': os << L"\\t"; break;
			case '\\': os << L"\\\\"; break;
			case '\'': os << L"\\'"; break;
			default:
				if (*s < ' ') os << "0" << std::oct << std::setw(3) << std::setfill(L'0') << int(*s);
				else os << *s;
			}
		os << '\'';
		return os;
	}
}
    