#include "typevar.h"

namespace OFEC {


	///
	/// ctor: init with none
	///
	typevar::typevar() : _var() {}

	///
	/// ctor: init with bool
	///
	typevar::typevar(bool n) : _var(n) {}

	///
	/// ctor: init with int
	///
	typevar::typevar(int n) : _var(n) {}

	///
	/// ctor: init with double
	///
	typevar::typevar(double n) : _var(n) {}

	///
	/// ctor: init with string
	///
	typevar::typevar(const std::string& s) : _var(string_t(s)) {}

	///
	/// ctor: init with string constant
	///
	typevar::typevar(const char* s) : _var(string_t(s)) {}

	///
	/// ctor: init with wide string
	///
	typevar::typevar(const std::wstring& s) : _var(wstring_t(s)) {}

	///
	/// ctor: init with wide string
	///
	typevar::typevar(const wchar_t* s) : _var(wstring_t(s)) {}

	///
	/// ctor: init with typevar
	///
	typevar::typevar(const typevar& v) : _var(v._var) {}

	typevar::typevar(char c) : _var(c) {}

	typevar::typevar(size_type n) : _var(n) {}
}