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
* class algorithm is an abstract for all algorithms.
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
		void run();
		
		bool terminated();
		virtual bool terminating();
		real duration();
		void set_termination(termination* t);
		const std::string& name() { return m_name; }
		void set_name(const std::string &name) { m_name = name; }
		virtual void initialize() = 0;
		virtual void record() = 0;
		const param_map& parameters()const { return m_parameters; }
		termination* get_termination() { return m_term.get(); }
	protected:
		virtual void run_() = 0;
		virtual void update_parameters();
		virtual void handle_evaluation_tag(evaluation_tag tag) {}
	protected:
		std::string m_name;
		std::unique_ptr<termination> m_term;
		param_map m_parameters;

	};

}
#endif // !OFEC_ALGORITHM_H
