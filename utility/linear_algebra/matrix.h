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
* class Matrix generates orthogonal rotation matrixs
*
*********************************************************************************/

#ifndef OFEC_MATRIX_H
#define OFEC_MATRIX_H

#include "vector.h"

namespace ofec {
	class Matrix final {								// *****************Orthogonal rotation matrix***********************
		friend Matrix operator*(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator+(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator-(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator*(const Real real, const Matrix &mat);
		friend Matrix operator*(const Matrix &mat, const Real real);
		friend Matrix operator/(const Real real, const Matrix &mat);
		friend Matrix operator/(const Matrix &mat, const Real real);
		friend Vector operator*(const Matrix &mat, const Vector &vec);
		friend std::ostream& operator<<(std::ostream &out, Matrix &m);

	public:
		Matrix(size_t dim = 0);
		Matrix(size_t r, size_t c);
		Matrix(const Matrix &m);
		Matrix(Matrix&& m);
		~Matrix();
		Matrix& operator*=(const Matrix &m);
		Matrix& operator+=(const Matrix &m);
		Matrix& operator-=(const Matrix &m);
		Matrix& operator*=(Real x);
		Matrix& operator=(const Matrix &m);
		Matrix& operator=(Matrix &&m);
		Vector& operator[](size_t idx);
		const Vector& operator[](size_t idx) const;

		size_t row() const { return m_row_size; }
		size_t col() const { return m_col_size; }

		bool equal(const Matrix &m, Real accuracy = 1e-6);
		bool checkIdentity(Real accuracy = 1e-6);
		bool checkDiagonal(Real accuracy = 1e-6);
		bool checkOrthogonal(Real accuracy = 1e-6);
		bool checkInvertible(Real accuracy = 1e-6);
		bool checkSymmetric(Real accuracy = 1e-6);

		void setRotationAxes(size_t r, size_t c, Real angle);
		void generateRotationModified(Normal *rand, Real CondiNum);
		void generateRotationClassical(Normal *rand, Real CondiNum);

		void randomize(Uniform *rand, Real min=-1, Real max=1);
		void randomize(Normal *rand);

		void identify();
		void modifiedOrthonormalize();
		void classicalOrthonormalize();
		void diagonalize(Normal *rand, Real CondiNum);
		Matrix transpose() const;
		void inverse();
		void diagonalize(Real alpha);
		void covarianceMatrix(Matrix &mat, Vector &mean);
		void eigenDecompose(Matrix &V, Vector &d);
		void choleskyDecompose(Matrix &cholesky);
		void choleskyDecomposeUnivariate(Matrix &cholesky);

		void zeroize();
		void setRow(const Real *d, size_t c, size_t r = 1);
		void setCol(const Real *d, size_t r, size_t c = 1);
		void set(const std::vector<Vector> &d);
		void set(const Real *const *d);
		
		void resize(size_t row, size_t col);
		Real determinant();
		std::vector<Vector>& data();
		Real** toRaw();
		void load(std::ifstream &in);
		void print(std::ofstream &out);
		void clear();
		void appendRow(const Vector &vec);

		Matrix sliceRow(const std::vector<int> &ids) const;
		Vector sliceCol(int idx) const;
		Matrix sliceCol(const std::vector<int> &ids) const;
		Matrix concatRow(const Matrix &mat) const;
		Matrix concatRow(const Vector &vec) const;
		Matrix concatCol(const Matrix &mat) const;

	private:
		void Householder2(size_t n, Matrix &V, Vector &d, Vector &e);
		void QLalgo2(size_t n, Matrix &V, Vector &d, Vector &e);
	
		Real determinant_(std::vector<Vector> &temp, size_t num);
		Real**choleskyDecompose_(Real **matrix, int n);

	private:
		size_t m_col_size, m_row_size;									// matrix size
		std::vector<Vector> m_row;
		Real** m_raw;
		Real m_determinant;
		bool m_det_flag;

	public:
		static Matrix diag(const Vector &vec);
		static Matrix eye(size_t n);
		static Matrix zeros(size_t row, size_t col);
		static Matrix colVec(const Vector &vec);
		static Matrix rowVec(const Vector &vec);
		static Matrix repVec(size_t n, const Vector &vec);
	};
}

namespace linpack {
	int DCHDC(ofec::Real a[], int lda, int p, ofec::Real work[], int ipvt[]);
}

namespace blas {
	int DSWAP(int n, ofec::Real *dx, int incx, ofec::Real *dy, int incy);
	int DAXPY(int n, ofec::Real da, ofec::Real *dx, int incx, ofec::Real *dy, int incy);
}

#endif
