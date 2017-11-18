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

#ifndef OFEC_CEC2013_FUNCTION_H
#define OFEC_CEC2013_FUNCTION_H

#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/matrix.h"

namespace OFEC {
	struct index_map {
		unsigned arr_index1;
		unsigned arr_index2;
	};
	class function_CEC2013 : public continuous {
	protected:
		unsigned ID;
		void create_shifted_vector(std::vector<real> &vec);
		void create_random_permutation(std::vector<size_t> &vec);
		void create_rotated_matrix(size_t dim, matrix & mat);
		void create_rotated_matrix_1D(matrix & mat, std::vector<real> & vec);
		void create_multi_rotated_matrix_1D(size_t dim, size_t num, std::vector<std::vector<real>> &vvec);

		
		// Basic mathematical functions' declaration
		real* multiply(real * vector, real * matrix, size_t dim);
		real* multiply(real * vector, real ** matrix, size_t dim);
		real elliptic(real * x, size_t dim);
		
		real rastrigin(real * x, size_t dim);
		real rastrigin(real * x, size_t dim, size_t k);
		real ackley(real * x, size_t dim);
		real ackley(real * x, size_t dim, size_t k);
		
		real rot_rastrigin(real * x, size_t dim);
		real rot_rastrigin(real * x, size_t dim, size_t k);
		real rot_ackley(real * x, size_t dim);
		real rot_ackley(real * x, size_t dim, size_t k);
		real schwefel(real * x, size_t dim);
		real schwefel(real * x, size_t dim, size_t k);
		real sphere(real * x, size_t dim);
		real sphere(real * x, size_t dim, size_t k);
		real rosenbrock(real * x, size_t dim);
		real rosenbrock(real * x, size_t dim, size_t k);
		unsigned convertMatrixToArrayIndex(unsigned i, unsigned j);
		void create_index_mapping();
	
		real* rotate_vector(size_t i, size_t &c);
		real* rotate_vector_conform(size_t i, size_t &c);
		real* rotate_vector_conflict(size_t i, size_t &c, real* x);

		void set_global_opt(real *tran = 0);
		void set_original_global_opt(real *opt = 0);

		real* mp_Ovector;
		size_t* mp_Pvector;
		real* mp_anotherz;
		real* mp_anotherz1;
		real* mp_anotherz2;
		real* mp_rot_matrix;
		size_t* mp_s;
		double* mp_w;

		real** mpp_OvectorVec;
		real** mpp_MultiRotmatrix1D;
		real** mpp_r25;
		real** mpp_r50;
		real** mpp_r100;

		bool mb_setOvectorToZero;

		size_t m_nonSeparableGroupNumber;
		size_t m_nonSeparableGroupSize;
		size_t m_overlap;

		std::vector<bool> mbv_interArray;
		struct index_map* mp_index_map;
		unsigned m_arrSize;

	public:
		//CEC2013(param_map &v);
		function_CEC2013(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~function_CEC2013();

		std::vector<bool> getInterArray();
		void ArrToMat(unsigned I1, unsigned I2, unsigned &matIndex);
		void MatToArr(unsigned &I1, unsigned &I2, unsigned matIndex);

		/* for CEC2013SS */
		real* readOvector();
		real** readOvectorVec();
		size_t* readPermVector();
		real** readR(size_t sub_dim);
		size_t* readS(size_t num);
		double* readW(size_t num);

		size_t* getS();

		void transform_osz(real* z, size_t dim);
		void transform_asy(real* z, double beta, size_t dim);
		void lambda(real* z, double alpha, size_t dim);
		int sign(double x);
		double hat(double x);
		double c1(double x);
		double c2(double x);

		optima<variable<real>, real> m_original_global_opt;
	};
}
#endif  // !OFEC_CEC2013_FUNCTION_H
