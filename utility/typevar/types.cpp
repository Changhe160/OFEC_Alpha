
#include "typevar.h"
namespace OFEC {
	///
	/// cast to bool
	///
	typevar::operator bool() const {
		try {
			return mapbox::util::get<bool_t>(_var);
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to bool");
		}
	}

	typevar::operator char() const {
		try {
			return mapbox::util::get<char_t>(_var);
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to char");
		}
	}

	///
	/// cast to int
	///
	typevar::operator int() const {
		try {
			return mapbox::util::get<int_t>(_var);
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to int");
		}
	}

	///
	/// cast to double
	///
	typevar::operator double() const {
		try {
			return mapbox::util::get<double_t>(_var);
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to double");
		}
	}

	///
	/// cast to string
	///
	typevar::operator std::string() const {
		try {
			return *mapbox::util::get<string_t>(_var).ps;
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to string");
		}
	}

	///
	/// cast to wide string
	///
	typevar::operator std::wstring() const {
		try {
			return *mapbox::util::get<wstring_t>(_var).ps;
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to wstring");
		}
	}

	typevar::operator size_type() const {
		try {
			if (is_size_t())     return mapbox::util::get<size_type>(_var);
			else if (is_int()) return static_cast<size_type>(mapbox::util::get<int>(_var));
		}
		catch (const mapbox::util::bad_variant_access&) {
			THROW("cannot convert to size_type");
		}
	}
	///
	/// @return type name
	///
	template<>
	struct typevar::name_visitor<std::string> 
	{
		using result_type = std::string;
		result_type operator () (const null_t&) const { return "null"; }
		result_type operator () (const bool_t&) const { return "bool"; }
		result_type operator () (const char_t&) const { return "char"; }
		result_type operator () (const int_t&) const { return "int"; }
		result_type operator () (const double_t&) const { return "double"; }
		result_type operator () (const string_t& value) const { return "string"; }
		result_type operator () (const wstring_t& value) const { return "wstring"; }
		result_type operator () (const size_type& ptr) const { return "size_type"; }
	};
	std::string typevar::name() const {
		return mapbox::util::apply_visitor(name_visitor<std::string>(), _var);
	}

	///
	/// @return type identifier
	///
	typevar::code typevar::type() const {
		return code(_var.which());
	}

}