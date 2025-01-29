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
*********************************************************************************/


// note that: only printable character can be printed, including 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
#ifndef OFEC_VARIANTS_TO_STREAM_H
#define OFEC_VARIANTS_TO_STREAM_H
#include "parameter_variant.h"
#include <sstream>
#include <map>
#include <tuple>
#include <queue>
#include <fstream>
#include <mutex>
#include <vector>
#include <iomanip>


namespace ofec {

	namespace variants_stream{



		void stringstream2parameterStream(std::stringstream& info,
			ParameterVariantStream& params);
		void parameterStream2stringstream(ParameterVariantStream& params, 
			std::stringstream& info,
			int precision = 6);

		void outputToFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath);
		void inputFromFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath);

	};
}



#endif