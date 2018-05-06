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
* class matrix generates orthogonal rotation matrixs
*
*********************************************************************************/

#ifndef OFEC_MATRIX_H
#define OFEC_MATRIX_H

#include "vector.h"

namespace OFEC {

	class matrix final {								// *****************Orthogonal rotation matrix***********************
	public:
		matrix(size_t dim = 0);
		matrix(size_t r, size_t c);
		matrix & operator*=(const matrix & m);
		matrix & operator*=(double x);
		matrix  operator*(const matrix & m);
		matrix & operator=(const matrix & m);
		matrix & operator=(matrix && m);
		matrix(matrix&& m);
		Vector & operator[](size_t idx);
		const Vector & operator[](size_t idx)const;

		bool eqaul(const matrix & m, double accuracy = 1e-6);
		bool identify();
		bool check_identity(double accuracy = 1e-6);
		bool check_diagonal(double accuracy = 1e-6);
		bool check_orthogonal(double accuracy = 1e-6);
		bool check_invertible(double accuracy = 1e-6);

		void set_rotation_axes(size_t r, size_t c, double angle);
		void generate_rotation_modified(normal *rand, double CondiNum);
		void generate_rotation_classical(normal *rand, double CondiNum);

		void randomize(uniform * rand, real min=-1, real max=1);
		void randomize(normal *rand);

		void modified_orthonormalize();
		void classical_orthonormalize();
		void diagonalize(normal * rand, double CondiNum);
		void transpose();
		void inverse();
		void diagonalize(double alpha);
		void eigendecomposition(std::vector<real> & eigen_value, std::vector<Vector> & eigen_vector);

		void zeroize();
		void set_row(const real *d, size_t c, size_t r = 1);
		void set_col(const real *d, size_t r, size_t c = 1);
		void set(const std::vector<Vector> & d);
		void set(const real * const * d);
		
		void resize(size_t row, size_t col);
		double determinant();
		std::vector<Vector>& data();
		void load(std::ifstream &in);
		void print(std::ofstream & out);
	private:
		void clear();
		void Householder2(size_t n, std::vector<Vector> & V, std::vector<real> & d, std::vector<real> & e);
		void QLalgo2(size_t n, std::vector<real> & d, std::vector<real> & e, std::vector<Vector> & V);
	
		double determinant_(std::vector<Vector>& temp, size_t num);
	private:
		size_t m_col_size, m_row_size;									// matrix size
		std::vector<Vector> m_row;
		bool m_det_flag = false;
		real m_det;
	};

}

#endif
