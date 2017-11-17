#include "typevar.h"

namespace OFEC {


	int typevar::operator+(int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) + n;
		THROW("invalid + to int@typevar::operator+");
	}
	double typevar::operator+(double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) + n;
		THROW("invalid +  to double@typevar::operator+");
	}

	typevar& typevar::operator++() {

		if (is_int()) {
			_var = mapbox::util::get<int>(_var) + 1;
		}
		else if (is_char()) {
			_var = static_cast<char>(mapbox::util::get<char>(_var) + 1);
		}
		else if (is_bool()) {
			_var = 1;
		}
		else {
			THROW("invalid ++ @typevar::operator++()");
		}

		return *this;
	}
	typevar typevar::operator++(int) {
		typevar x(*this);
		++(*this);

		return x;
	}
	typevar typevar::operator+(const typevar& v)const {
		switch (type()) {
		case type_null:    THROW("invalid + to none");
		case type_bool:    THROW("bool + not implemented");
		case type_int:    if (v.is_int()) return mapbox::util::get<int_t>(_var) + mapbox::util::get<int_t>(v._var);
			THROW("invalid + to int");
		case type_double:  if (v.is_double()) return mapbox::util::get<double_t>(_var) + mapbox::util::get<double_t>(v._var);
			THROW("invalid + to double");
		case type_string:  THROW("string + not implemented");
		case type_wstring: THROW("wstring + not implemented");
		default:           THROW("(unhandled type) + not implemented");
		}
	}

	int typevar::operator-(int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) - n;
		THROW("invalid - to int@typevar::operator-");
	}
	double typevar::operator-(double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) - n;
		THROW("invalid - to double@typevar::operator-");
	}
	typevar typevar::operator-(const typevar& v)const {
		switch (type()) {
		case type_null:    THROW("invalid - comparison to none");
		case type_bool:    THROW("bool - not implemented");
		case type_int:    if (v.is_int()) return mapbox::util::get<int_t>(_var) - mapbox::util::get<int_t>(v._var);
			THROW("invalid - to int");
		case type_double:  if (v.is_double()) return mapbox::util::get<double_t>(_var) - mapbox::util::get<double_t>(v._var);
			THROW("invalid - to double");
		case type_string:  THROW("string - not implemented");
		case type_wstring: THROW("wstring - not implemented");
		default:           THROW("(unhandled type) - not implemented");
		}
	}


	int typevar::operator*(int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) * n;
		THROW("invalid * to int@typevar::operator*");
	}
	double typevar::operator*(double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) * n;
		THROW("invalid * to double@typevar::operator*");
	}
	typevar typevar::operator*(const typevar& v)const {
		switch (type()) {
		case type_null:    THROW("invalid * comparison to none");
		case type_bool:    THROW("bool * not implemented");
		case type_int:    if (v.is_int()) return mapbox::util::get<int_t>(_var) * mapbox::util::get<int_t>(v._var);
			THROW("invalid * to int");
		case type_double:  if (v.is_double()) return mapbox::util::get<double_t>(_var) * mapbox::util::get<double_t>(v._var);
			THROW("invalid * to double");
		case type_string:  THROW("string * not implemented");
		case type_wstring: THROW("wstring * not implemented");
		default:           THROW("(unhandled type) * not implemented");
		}
	}


	int typevar::operator/(int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) / n;
		THROW("invalid /  to int@typevar::operator/");
	}
	double typevar::operator/(double n) const {
		if (is_double()) return mapbox::util::get<double_t>(_var) / n;
		THROW("invalid /  to double@typevar::operator/");
	}
	typevar typevar::operator/(const typevar& v)const {
		switch (type()) {
		case type_null:    THROW("invalid /  to none");
		case type_bool:    THROW("bool / not implemented");
		case type_int:    if (v.is_int()) return mapbox::util::get<int_t>(_var) / mapbox::util::get<int_t>(v._var);
			THROW("invalid / to int");
		case type_double:  if (v.is_double()) return mapbox::util::get<double_t>(_var) / mapbox::util::get<double_t>(v._var);
			THROW("invalid / to double");
		case type_string:  THROW("string / not implemented");
		case type_wstring: THROW("wstring / not implemented");
		default:           THROW("(unhandled type) / not implemented");
		}
	}


	int typevar::operator%(int n) const {
		if (is_int()) return mapbox::util::get<int_t>(_var) % n;
		THROW("invalid % to int@typevar::operator%");
	}
	typevar typevar::operator%(const typevar& v)const {
		switch (type()) {
		case type_null:    THROW("invalid % to none");
		case type_bool:    THROW("invalid % not implemented");
		case type_int:    if (v.is_int()) return mapbox::util::get<int_t>(_var) % mapbox::util::get<int_t>(v._var);
			THROW("invalid % to int");
		case type_double:  THROW("invalid % to double");
		case type_string:  THROW("invalid % to string");
		case type_wstring: THROW("invalid % to wstring");
		case type_size_t:  if (v.is_size_t()) return mapbox::util::get<size_type>(_var) % mapbox::util::get<size_type>(v._var);
			THROW("invalid % to size_t");
		default:           THROW("(unhandled type) % not implemented");
		}
	}

	typevar::size_type typevar::operator-(size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) - n;
		THROW("invalid -  to int@typevar::operator-");
	}
	typevar::size_type typevar::operator+(size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) + n;
		THROW("invalid +  to int@typevar::operator+");
	}
	typevar::size_type typevar::operator*(size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) * n;
		THROW("invalid *  to int@typevar::operator*");
	}
	typevar::size_type typevar::operator/(size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) / n;
		THROW("invalid /  to int@typevar::operator/");
	}
	typevar::size_type typevar::operator%(size_type n) const {
		if (is_size_t()) return mapbox::util::get<size_type>(_var) % n;
		THROW("invalid %  to int@typevar::operator%");
	}
}