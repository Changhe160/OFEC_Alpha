/********* Begin Register Information **********
{
	"name": "GOP_CEC2005_F01",
	"identifier": "GOP_CEC2005_F01",
	"tags": [ "single-objective", "continuous" ]
}
*********** End Register Information **********/

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_F1_SHIFTED_SPHERE_H
#define OFEC_F1_SHIFTED_SPHERE_H

#include "../classical/sphere.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedSphere : public Sphere {
			OFEC_CONCRETE_INSTANCE(ShiftedSphere)
		protected:
			void addInputParameters() {}
			void initialize_(Environment *env) override;
			void updateOptima(Environment *env) override;
		};
	}
	using GOP_CEC2005_F01 = cec2005::ShiftedSphere;
}
#endif // ! OFEC_F1_SHIFTED_SPHERE_H
