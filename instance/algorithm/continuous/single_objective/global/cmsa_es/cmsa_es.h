/********* Begin Register Information **********
{
    "description": "",
    "identifier": "CMSA_ES",
    "name": "CMSA-ES",
    "tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

/***************************************************
@inproceedings{beyer2008covariance,
  pages = {123--132},
  year = {2008},
  author = {Hans-Georg Beyer and Bernhard Sendhoff},
  organization = {Springer},
  title = {Covariance matrix adaptation revisited--the {CMSA} evolution strategy--},
  booktitle = {International Conference on Parallel Problem Solving from Nature}
}
***************************************************/

#ifndef OFEC_CMSA_ES_H
#define OFEC_CMSA_ES_H

#include "../../../../../../core/algorithm/algorithm.h"

namespace ofec {
	class CMSA_ES : virtual public Algorithm {
		OFEC_CONCRETE_INSTANCE(CMSA_ES)
	protected:
		size_t m_pop_size;

		void addInputParameters();
		void initialize_(Environment *env) override;
		void run_(Environment *env) override;
	};
}

#endif // ! OFEC_CMSA_ES_H
