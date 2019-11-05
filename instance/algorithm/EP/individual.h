/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created by Junchen Wang on Oct. 28, 2018.

#ifndef OFEC_EP_IND_H
#define OFEC_EP_IND_H

#include "../../../core/algorithm/individual.h"

namespace OFEC {
	namespace EP {
		class individual : public OFEC::individual<> {
		protected:
			std::vector<real> m_eta;
			void initialize_eta();
		public:
			individual(size_t num_obj, size_t num_cont, size_t size_var);
			void resize_variable(size_t n);
			std::vector<real>& eta() { return m_eta; }
			const std::vector<real>& eta()const { return m_eta; }
		};
	}
}


#endif // !OFEC_EP_IND_H

