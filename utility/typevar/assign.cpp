
#include "typevar.h"

namespace OFEC {


	///
	/// assign bool to typevar
	///
	typevar& typevar::operator = (bool n) {
		_var = n;
		return *this;
	}

	typevar& typevar::operator = (char c) {
		_var = c;
		return *this;
	}
	///
	/// assign int to typevar
	///
	typevar& typevar::operator = (int n) {
		_var = n;
		return *this;
	}


	///
	/// assign double to typevar
	///
	typevar& typevar::operator = (double n) {
		_var = n;
		return *this;
	}

	///
	/// assign string to typevar
	///
	typevar& typevar::operator = (const std::string& s) {
		_var = string_t(s);
		return *this;
	}

	///
	/// assign string constant to typevar
	///
	typevar& typevar::operator = (const char* s) {
		_var = string_t(s);
		return *this;
	}

	///
	/// assign wide string to typevar
	///
	typevar& typevar::operator = (const std::wstring& s) {
		_var = wstring_t(s);
		return *this;
	}

	///
	/// assign wide string constant to typevar
	typevar& typevar::operator = (const wchar_t* s) {
		_var = wstring_t(s);
		return *this;
	}

	///
	/// assign typevar to typevar
	///
	typevar& typevar::operator = (const typevar& v) {
		_var = v._var;
		return *this;
	}

	typevar& typevar::operator += (int n) {
		if (is_int()) _var = mapbox::util::get<int>(_var) + n;
		else THROW("invalid += to int @typevar::operator+=");
		return *this;
	}
	typevar& typevar::operator += (double n) {
		if (is_double()) _var = mapbox::util::get<double>(_var) + n;
		else THROW("invalid += to double @typevar::operator+=");
		return *this;
	}
	typevar& typevar::operator -= (int n) {
		if (is_int()) _var = mapbox::util::get<int>(_var) - n;
		THROW("invalid -= to int @typevar::operator-=");
		return *this;
	}
	typevar& typevar::operator -= (double n) {
		if (is_double()) _var = mapbox::util::get<double>(_var) - n;
		else THROW("invalid -= to double @typevar::operator-=");
		return *this;
	}
	typevar& typevar::operator *= (int n) {
		if (is_int()) _var = mapbox::util::get<int>(_var)*n;
		else THROW("invalid *= to int @typevar::operator*=");
		return *this;
	}
	typevar& typevar::operator *= (double n) {
		if (is_double()) _var = mapbox::util::get<double>(_var)*n;
		else THROW("invalid *= to double @typevar::operator*=");
		return *this;
	}
	typevar& typevar::operator /= (int n) {
		if (is_int()) _var = mapbox::util::get<int>(_var) / n;
		else THROW("invalid /= to int @typevar::operator/=");
		return *this;
	}
	typevar& typevar::operator /= (double n) {
		if (is_double()) _var = mapbox::util::get<double>(_var) / n;
		else THROW("invalid /= to double @typevar::operator/=");
		return *this;
	}
	typevar& typevar::operator %= (int n) {
		if (is_int()) _var = mapbox::util::get<int>(_var) % n;
		else THROW("invalid %= to int @typevar::operator%=");
		return *this;
	}

	typevar& typevar::operator = (size_type n) {
		_var = n;
		return *this;
	}

	typevar& typevar::operator += (size_type n) {
		if (is_size_t()) _var = mapbox::util::get<size_type>(_var) + n;
		else THROW("invalid += to size_type @typevar::operator+=");
		return *this;
	}
	typevar& typevar::operator -= (size_type n) {
		if (is_size_t()) _var = mapbox::util::get<size_type>(_var) - n;
		else THROW("invalid %= to size_type @typevar::operator-=");
		return *this;
	}
	typevar& typevar::operator *= (size_type n) {
		if (is_size_t()) _var = mapbox::util::get<size_type>(_var)*n;
		else THROW("invalid %= to size_type @typevar::operator*=");
		return *this;
	}
	typevar& typevar::operator /= (size_type n) {
		if (is_size_t()) _var = mapbox::util::get<size_type>(_var) / n;
		else THROW("invalid %= to size_type @typevar::operator/=");
		return *this;
	}
	typevar& typevar::operator %= (size_type n) {
		if (is_size_t()) _var = mapbox::util::get<size_type>(_var) % n;
		else THROW("invalid %= to size_type @typevar::operator%=");
		return *this;
	}
}