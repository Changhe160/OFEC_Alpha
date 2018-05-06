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
* This file includes types for special purpose
*
*********************************************************************************/

#ifndef OFEC_TYPES_H
#define OFEC_TYPES_H
#include "typevar/typevar.h"

class singleton {
protected:
	singleton() = default;
	~singleton() = default;
	singleton(const singleton&) = delete;
	singleton& operator=(const singleton &) = delete;
};
#endif