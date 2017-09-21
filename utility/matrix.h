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
		matrix(int dim = 0);
		matrix(const int r, const int c);
		~matrix();
		matrix & operator*=(const matrix & m);
		matrix & operator*=(double x);
		matrix  operator*(const matrix & m);
		matrix & operator=(const matrix & m);
		bool eqaul(const matrix & m, double accuracy = 1e-6);
		bool identify();
		bool is_identity(double accuracy = 1e-6);
		void set_rotation_axes(int r, int c, double angle);
		void modified_generate_rotation(normal *rand, double CondiNum);
		void classical_generate_rotation(normal *rand, double CondiNum);
		void randomize(uniform * rand, real min=-1, real max=1);
		void randomize(normal *rand);
		void modified_orthonormalize();
		void classical_orthonormalize();
		void zero();
		void set_row(const real *d, int c, int r = 1);
		void set_col(const real *d, const int r, const int c = 1);
		void set(const std::vector<Vector> & d);
		void set(const real * const * d);
		void diagonalize(normal * rand, double CondiNum);
		void transpose();
		void inverse();
		bool is_inverse(double accuracy = 1e-6);
		Vector & operator[](int idx);
		const Vector & operator[](int idx)const;
		void resize(int row, int col);
		void diagonalize(double alpha);
		bool is_diagonal(double accuracy = 1e-6);
		bool is_orthogonal(double accuracy = 1e-6);
		double determinant_(std::vector<Vector>& temp, int num);
		double determinant();
		std::vector<Vector>& data();
		/* Eigendecomposition of a matrix */
		void eigendecomposition(std::vector<double> & eigen_value, std::vector<Vector> & eigen_vector);
		/* * * * * * * * * * * * * * * * * */
		void load(std::ifstream &in);
		void print(std::ofstream & out);
			
	private:
		void clear();
		void Householder2(int n, std::vector<Vector> & V, std::vector<double> & d, std::vector<double> & e);
		void QLalgo2(int n, std::vector<double> & d, std::vector<double> & e, std::vector<Vector> & V);
		double myhypot(double a, double b);
	private:
		int m_col_size, m_row_size;									// matrix size
		std::vector<Vector> m_row;
		bool m_is_det_flag = false;
		double m_det;
	};

}

#endif
