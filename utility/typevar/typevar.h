/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class typevar wraps boost::variant type for handling different types of parameter values 
*
*********************************************************************************/

#ifndef OFEC_DYNAMIC_VAR_H
#define OFEC_DYNAMIC_VAR_H
#include <string>
#include <map>

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


#include "../myexcept.h"
#include "../../core/definition.h"


namespace OFEC {
	class typevar {
	public:
		using size_type = std::size_t;
		// Note to dynamic developer: Make sure that code and the variant list for var_t always match
		enum code { type_null = 0, type_bool, type_char, type_int, type_double, type_string, type_wstring, type_size_t };

		typevar();
		typevar(bool);
		typevar(char c);
		typevar(int n);
		typevar(double n);
		typevar(const std::string& s);
		typevar(const char* s);
		typevar(const std::wstring& s);
		typevar(const wchar_t* s);
		typevar(const typevar& v);
		typevar(size_type n);

		typevar& operator = (bool);
		typevar& operator = (char c);
		typevar& operator = (int n);
		typevar& operator = (double n);
		typevar& operator = (const std::string& s);
		typevar& operator = (const char* s);
		typevar& operator = (const std::wstring& s);
		typevar& operator = (const wchar_t* s);
		typevar& operator = (const typevar& v);
		typevar& operator = (size_type n);

		typevar& operator += (int n);
		typevar& operator += (double n);
		typevar& operator -= (int n);
		typevar& operator -= (double n);
		typevar& operator *= (int n);
		typevar& operator *= (double n);
		typevar& operator /= (int n);
		typevar& operator /= (double n);
		typevar& operator %= (int n);

		typevar& operator += (size_type n);
		typevar& operator -= (size_type n);
		typevar& operator *= (size_type n);
		typevar& operator /= (size_type n);
		typevar& operator %= (size_type n);


		operator bool() const;
		operator char() const;
		operator int() const;
		operator double() const;
		operator std::string() const;
		operator std::wstring() const;
		operator size_type () const;

		enum code type() const;
		std::string name() const;

		int operator+(int) const;
		double operator+(double) const;
		typevar operator+(const typevar&)const;

		//waring: there is no range management 
		typevar& operator++();
		typevar operator++(int);

		int operator-(int) const;
		double operator-(double) const;
		typevar operator-(const typevar&)const;

		int operator*(int) const;
		double operator*(double) const;
		typevar operator*(const typevar&)const;

		int operator/(int) const;
		double operator/(double) const;
		typevar operator/(const typevar&)const;


		int operator%(int) const;
		typevar operator%(const typevar&)const;

		size_type operator-(size_type n) const;
		size_type operator+(size_type n) const;
		size_type operator*(size_type n) const;
		size_type operator/(size_type n) const;
		size_type operator%(size_type n) const;

		bool operator == (bool) const;
		bool operator == (char c) const;
		bool operator == (int n) const;
		bool operator == (double n) const;
		bool operator == (const std::string& s) const;
		bool operator == (const char* s) const;
		bool operator == (const std::wstring& s) const;
		bool operator == (const wchar_t* s) const;
		bool operator == (const typevar& v) const;
		bool operator == (size_type n) const;

		bool operator != (bool) const;
		bool operator != (char) const;
		bool operator != (int n) const;
		bool operator != (double n) const;
		bool operator != (const std::string& s) const;
		bool operator != (const char* s) const;
		bool operator != (const std::wstring& s) const;
		bool operator != (const wchar_t* s) const;
		bool operator != (const typevar& v) const;
		bool operator != (size_type n) const;


		bool operator < (bool) const;
		bool operator < (char) const;
		bool operator < (int n) const;
		bool operator < (double n) const;
		bool operator < (const std::string& s) const;
		bool operator < (const char* s) const;
		bool operator < (const std::wstring& s) const;
		bool operator < (const wchar_t* s) const;
		bool operator < (const typevar& v) const;
		bool operator < (size_type n) const;

		bool operator <= (bool) const;
		bool operator <= (char) const;
		bool operator <= (int n) const;
		bool operator <= (double n) const;
		bool operator <= (const std::string& s) const;
		bool operator <= (const char* s) const;
		bool operator <= (const std::wstring& s) const;
		bool operator <= (const wchar_t* s) const;
		bool operator <= (const typevar& v) const;
		bool operator <= (size_type n) const;

		bool operator > (bool) const;
		bool operator > (char) const;
		bool operator > (int n) const;
		bool operator > (double n) const;
		bool operator > (const std::string& s) const;
		bool operator > (const char* s) const;
		bool operator > (const std::wstring& s) const;
		bool operator > (const wchar_t* s) const;
		bool operator > (const typevar& v) const;
		bool operator > (size_type n) const;

		bool operator >= (bool) const;
		bool operator >= (char n) const;
		bool operator >= (int n) const;
		bool operator >= (double n) const;
		bool operator >= (const std::string& s) const;
		bool operator >= (const char* s) const;
		bool operator >= (const std::wstring& s) const;
		bool operator >= (const wchar_t* s) const;
		bool operator >= (const typevar& v) const;
		bool operator >= (size_type n) const;

		/// is typevar a null?
		bool is_null() const { return type() == type_null; }
		/// is typevar a bool?
		bool is_bool() const { return type() == type_bool; }

		bool is_char() const { return type() == type_char; }
		/// is typevar an int?
		bool is_int() const { return type() == type_int; }
		/// is typevar a double?
		bool is_double() const { return type() == type_double; }
		/// is typevar a numeric type?
		bool is_numeric() const { return is_int() || is_double(); }
		/// is typevar a string?
		bool is_string() const { return type() == type_string; }
		/// is typevar a wide string?
		bool is_wstring() const { return type() == type_wstring; }
		/// is typevar a string type?
		bool is_string_type() const { return is_string() || is_wstring(); }

		bool is_size_t() const { return type() == type_size_t; }


		std::ostream& _write_var(std::ostream& os) const;
		std::ostream& _write_string(std::ostream& os) const;
		std::ostream& _write_wstring(std::ostream& os) const;

		std::wostream& _write_var(std::wostream& os) const;
		std::wostream& _write_string(std::wostream& os) const;
		std::wostream& _write_wstring(std::wostream& os) const;

		size_type count() const;

		/// typevar comparison functor
		struct less_var {
			/// typevar comparison function
			bool operator () (const typevar& lhs, const typevar& rhs);
		};


	private:
		friend	int operator-(int n, const typevar& v);
		friend	double operator-(double n, const typevar& v);
		friend	int operator/(int n, const typevar& v);
		friend	double operator/(double n, const typevar& v);
		friend int operator%(int n, const typevar& v);

		friend	int operator+(int n, const typevar& v);
		friend	double operator+(double n, const typevar& v);
		friend	int operator*(int n, const typevar& v);
		friend	double operator*(double n, const typevar& v);

		friend	void operator-=(int& n, const typevar& v);
		friend	void operator-=(double &n, const typevar& v);
		friend	void operator/=(int &n, const typevar& v);
		friend	void operator/=(double &n, const typevar& v);
		friend void operator%=(int& n, const typevar& v);
		friend	void operator+=(int& n, const typevar& v);
		friend	void operator+=(double &n, const typevar& v);
		friend	void operator*=(int & n, const typevar& v);
		friend	void operator*=(double & n, const typevar& v);

		typedef boost::blank null_t;

		struct string_t {
			string_t() : ps(boost::make_shared<std::string>()) {}
			string_t(const std::string& s) : ps(boost::make_shared<std::string>(s)) {}
			string_t(const char* s) : ps(boost::make_shared<std::string>(s)) {}

			boost::shared_ptr<std::string>  ps;
		};

		struct wstring_t {
			wstring_t() : ps(boost::make_shared<std::wstring>()) {}
			wstring_t(const std::wstring& s) : ps(boost::make_shared<std::wstring>(s)) {}
			wstring_t(const wchar_t* s) : ps(boost::make_shared<std::wstring>(s)) {}

			boost::shared_ptr<std::wstring>  ps;
		};

		typedef bool bool_t;
		typedef int int_t;
		typedef double double_t;
		typedef char char_t;
		typedef boost::variant<null_t, bool_t, char_t, int_t, double_t, string_t, wstring_t, size_type> var_t;

		var_t _var;

		struct name_visitor;
		struct count_visitor;
		struct equal_visitor;

	};

	/// ostream << typevar
	inline std::ostream& operator << (std::ostream& os, const typevar& v) { return v._write_var(os); }
	/// wostream << typevar
	inline std::wostream& operator << (std::wostream& os, const typevar& v) { return v._write_var(os); }


	inline	int operator-(int n, const typevar& v) {
		if (v.is_int()) return n - boost::get<int>(v._var);
		THROW("invalid - from int to var @ int operator-");
	}
	inline	double operator-(double n, const typevar& v) {
		if (v.is_double()) return n - boost::get<double>(v._var);
		THROW("invalid - from double to var @ double operator-");
	}

	inline	int operator/(int n, const typevar& v) {
		if (v.is_int()) return n / boost::get<int>(v._var);
		THROW("invalid / from int to var @ int operator /");
	}
	inline	double operator/(double n, const typevar& v) {
		if (v.is_double()) return n / boost::get<double>(v._var);
		THROW("invalid / from double to var @ double operator /");
	}

	inline	int operator%(int n, const typevar& v) {
		if (v.is_int()) return n%boost::get<int>(v._var);
		THROW("invalid % from int to var @ int operator %");
	}

	inline int operator+(int n, const typevar& v) {
		if (v.is_int()) return n + boost::get<int>(v._var);
		THROW("invalid + from int to var @ int operator +");
	}
	inline double operator+(double n, const typevar& v) {
		if (v.is_double()) return n + boost::get<double>(v._var);
		THROW("invalid + from double to var @ double operator +");
	}
	inline int operator*(int n, const typevar& v) {
		if (v.is_int()) return n*boost::get<int>(v._var);
		THROW("invalid * from int to var @ int operator *");
	}
	inline double operator*(double n, const typevar& v) {
		if (v.is_double()) return n*boost::get<double>(v._var);
		THROW("invalid * from double to var @ double operator *");
	}

	inline void operator-=(int& n, const typevar& v) {
		if (v.is_int())  n -= boost::get<int>(v._var);
		else THROW("invalid -= from int to var @ int operator -=");
	}
	inline void operator-=(double &n, const typevar& v) {
		if (v.is_double()) n -= boost::get<double>(v._var);
		else THROW("invalid -= from double to var @ double operator -=");
	}
	inline void operator/=(int &n, const typevar& v) {
		if (v.is_int())  n /= boost::get<int>(v._var);
		else THROW("invalid /= from int to var @ int operator /=");
	}
	inline void operator/=(double &n, const typevar& v) {
		if (v.is_double()) n /= boost::get<double>(v._var);
		else THROW("invalid /= from double to var @ double operator /=");
	}
	inline void operator%=(int& n, const typevar& v) {
		if (v.is_int())  n %= boost::get<int>(v._var);
		else THROW("invalid %= from int to var @ int operator %=");
	}
	inline void operator+=(int& n, const typevar& v) {
		if (v.is_int())  n += boost::get<int>(v._var);
		else THROW("invalid += from int to var @ int operator +=");
	}
	inline void operator+=(double &n, const typevar& v) {
		if (v.is_double()) n += boost::get<double>(v._var);
		else THROW("invalid += from double to var @ double operator +=");
	}
	inline void operator*=(int & n, const typevar& v) {
		if (v.is_int())  n *= boost::get<int>(v._var);
		else THROW("invalid *= from int to var @ int operator *=");
	}
	inline void operator*=(double & n, const typevar& v) {
		if (v.is_double()) n *= boost::get<double>(v._var);
		else THROW("invalid *= from double to var @ double operator *=");
	}

	using param_map = std::map<parameter, typevar>;

}

#endif /* DYNAMIC_VAR_HPP */
