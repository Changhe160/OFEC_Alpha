/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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
* A simple myexcept class
*
*********************************************************************************/

#ifndef OFEC_EXCEPTION_H
#define OFEC_EXCEPTION_H

#include <iostream>
#include <string>
#include <exception>


namespace ofec {
	class Exception: public std::exception {
	public :
		Exception(const std::string &message) {
			m_message = message;
#ifdef OFEC_UNIT_TEST
			std::cout << m_message << std::endl;
#endif
		}
		~Exception() noexcept = default;
		const std::string& whatStr() const noexcept { return m_message; }

		virtual const char* what() const noexcept {
			return m_message.c_str();
		}
	private :
		std::string m_message;
	};

#define THROW(mess) throw Exception(mess);
}

#endif                           

