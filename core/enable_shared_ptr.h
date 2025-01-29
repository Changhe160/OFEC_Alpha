/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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
* macros used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_ENABLE_SHARED_PTR_H
#define OFEC_ENABLE_SHARED_PTR_H


#include <memory>
#include "exception.h"

namespace ofec {
	template<typename T>
	class EnableSharedPtr : public std::enable_shared_from_this<T> {
	public:
		bool isSharedPtr() {
			return this->weak_from_this().use_count() != 0;
		}
		std::shared_ptr<T> getSharedPtr() {
			if (!isSharedPtr()) {
				throw Exception("not a shared ptr.");
			}
			return this->shared_from_this();
		}
		
	};
}



#endif // OFEC_ENABLE_SHARE_PTR_H
