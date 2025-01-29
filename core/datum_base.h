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
* An extended real type
*
*
*********************************************************************************/

#ifndef OFEC_DATUM_BASE_H
#define OFEC_DATUM_BASE_H

namespace ofec {
	class Algorithm;

	class DatumBase {
		friend class Algorithm;
	private:
		bool m_update_flag = false;
	public:
		virtual ~DatumBase() = default;
		bool updateFlag() const {
			return m_update_flag;
		}
	};
}


#endif // !OFEC_DATUM_BASE_H
