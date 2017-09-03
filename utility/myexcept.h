
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
* A simple myexcept class
*
*********************************************************************************/

#ifndef USING_myexcept_H
#define USING_myexcept_H

///
/// myexcept class thrown by OFEC
///
namespace OFEC {
	class myexcept {
	public :
		myexcept(const char* message)  { 
			m_message = message; 
		}
		~myexcept() noexcept {}
		const char* what() const noexcept { return m_message; }
	private :
		const char* m_message;
	};

#define THROW(mess) throw myexcept(mess);


}

#endif                            // end of USING_myexcept_H

