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
* class algorithm is the base for all algorithms.
*
*********************************************************************************/
#ifndef OFEC_ALGORITHM_H
#define OFEC_ALGORITHM_H

#include <sstream>
#include "termination.h"


namespace OFEC {
	class algorithm
	{
	public:
		algorithm() = default;
		explicit algorithm(const std::string);
		algorithm& operator=(const algorithm&) = default;
		algorithm& operator=(algorithm&&) = default;
		algorithm(const algorithm&) = default;
		algorithm(algorithm&&) = default;

		virtual ~algorithm() {}
		evaluation_tag run();
		
		bool terminated();
		virtual bool terminating();
		double duration();
		void set_termination(termination* t);
		const std::string& name() { return m_name; }
		void set_name(const std::string &name) { m_name = name; }
	protected:
		virtual evaluation_tag run_() { return evaluation_tag::Normal; }
	protected:
		std::string m_name;
		std::unique_ptr<termination> m_term;
		std::stringstream m_parameter;

	};

}
#endif // !OFEC_ALGORITHM_H
