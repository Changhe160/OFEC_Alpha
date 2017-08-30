
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
* A simple exception class
*
*********************************************************************************/

#ifndef USING_EXCEPTION_H
#define USING_EXCEPTION_H

///
/// exception class thrown by OFEC
///
namespace OFEC {
	class exception {
	public :
		exception(const char* message)  { m_message = message; }
		~exception() noexcept {}
		const char* what() const noexcept { return m_message; }
	private :
		const char* m_message;
	};
}

#endif                            // end of USING_EXCEPTION_H

