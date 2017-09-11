#include "matrix.h"
#include "myexcept.h"
#include "../core/global.h"


namespace OFEC {

	matrix::matrix(int dim) :m_col(dim), m_row(dim) {
		if (m_row == 0) return;
		allocateMemory(m_row, m_col);

	}
	matrix::matrix(const int c, const int r) : m_col(c), m_row(r) {
		allocateMemory(m_row, m_col);

	}
	void matrix::setDataRow(const double *d, const int c, const int r) {
		if (r != m_row) return;
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				m_vec[i][j] = d[j];
	}
	void matrix::setDataCol(const double *d, const int r, const int c) {
		if (c != m_col) return;
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				m_vec[i][j] = d[i];

	}
	void matrix::setData(const double * const * d) {
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				m_vec[i][j] = d[i][j];
	}
	matrix::~matrix() {

		freeMemory();
	}

	int matrix::getRow() {
		return m_row;
	}

	int matrix::getCol() {
		return m_col;
	}

	matrix & matrix::operator *=(const matrix &m) {

		if (m_col != m.m_row) throw myexcept("can not *, m_col of 1st matrix must be equal to m_row of 2nd matrix@ matrix::operator *=");

		matrix r(m.m_col, m_row);
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m.m_col; j++) {

				r.m_vec[i][j] = 0;
				for (int k = 0; k < m_col; k++)
					r.m_vec[i][j] += m_vec[i][k] * m.m_vec[k][j];
			}
		}
		if (m_row != m.m_row || m_col != m.m_col) {
			freeMemory();
			m_row = r.m_row; m_col = r.m_col;
			allocateMemory(m_row, m_col);
		}
		copy(r.m_vec.begin(), r.m_vec.end(), this->m_vec.begin());

		return *this;
	}
	matrix & matrix::operator *=(const double x) {
		for (int i = 0; i < m_row; i++) {
			m_vec[i] *= x;
		}
		return *this;
	}

	matrix&matrix::operator =(const matrix &m) {
		if (m_row != m.m_row || m_col != m.m_col) {
			throw myexcept("Assiment operator matrix@matrix::operator =");
		}
		if (this == &m) return *this;
		m_vec = m.m_vec;
		return *this;
	}
	bool matrix::identity() {
		if (m_row != m_col) return false;
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				m_vec[i][j] = (j == i);

		return true;
	}
	bool matrix::isIdentity() {
		if (m_row != m_col) return false;
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++) {
				if (m_vec[i][j] != (i == j ? 1. : 0)) {
					return false;
				}
			}
		}
		return true;
	}
	void matrix::diagonalize(const double CondiNum) {
		if (m_row != m_col) throw myexcept("can not be diagonal, matrix must be squre@matrix::diagonalize");

		double min, max;
		double *r = new double[m_row];
		for (int i = 0; i < m_row; i++)	r[i] = global::ms_global->getRandFloat(1, m_row, caller::Problem);

		min = max = r[0];
		for (int i = 0; i < m_row; i++) {
			if (min > r[i])min = r[i];
			if (max < r[i]) max = r[i];
		}
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				if (j != i) m_vec[i][j] = 0.;
				else m_vec[i][j] = pow(CondiNum, (r[i] - min) / (max - min));
				delete[]r;
				r = 0;
	}
	void matrix::setToZero() {
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < m_col; j++)
				m_vec[i][j] = 0.;
	}
	void matrix::setRotationAngle(const int r, const int c, const double angle) {
		identity();
		m_vec[r][r] = cos(angle);
		m_vec[r][c] = -sin(angle);
		m_vec[c][r] = sin(angle);
		m_vec[c][c] = cos(angle);
	}


	void matrix::generateRotationmatrix(const double CondiNum, caller rMode) {

		matrix ortholeft(m_row), orthoright(m_row), diagonal(m_row);
		ortholeft.randomize(rMode);
		ortholeft.orthonormalize();
		orthoright.randomize(rMode);
		orthoright.orthonormalize();
		diagonal.diagonalize(CondiNum);
		ortholeft *= diagonal;
		ortholeft *= orthoright;
		*this = ortholeft;

	}
	void matrix::randomize(caller rMode) {
		for (int i = 0; i < m_row; i++) m_vec[i].randomize(-1, 1, rMode);
	}
	void matrix::orthonormalizeClassical() {
		if (m_row != m_col) throw myexcept("cannot perform orthonormalization, matrix must be squre@matrix::orthonormalize");

		myVector t1(m_col), t2(m_col);
		//Gram schmidt process
		for (int i = 1; i < m_row; i++) {
			t1 = m_vec[i];
			t2 = m_vec[i];
			for (int j = 0; j < i; j++) {
				t1.projectionToV(m_vec[j]);
				t2 -= t1;
				t1 = m_vec[i];
			}
			m_vec[i] = t2;
		}
		// normalize each vector
		for (int i = 0; i < m_row; i++) m_vec[i].normalize();
	}

	void matrix::orthonormalize() {
		if (m_row != m_col) throw myexcept("cannot perform orthonormalization, matrix must be squre@matrix::orthonormalize");

		myVector v(m_col), q(m_col);
		double r = 0;
		//Gram schmidt process
		for (int i = 0; i < m_row; i++) {
			v = m_vec[i];
			q = m_vec[i];
			for (int j = 0; j < i; j++) {
				r = v * m_vec[j];
				v -= m_vec[j] * r;
			}
			r = v.length();
			m_vec[i] = v / r;
		}
		// normalize each vector
		for (int i = 0; i < m_row; i++) m_vec[i].normalize();
	}

	bool matrix::isOrthogonal() {
		if (m_row != m_col) throw myexcept("cannot identify, matrix must be squre@matrix::orthonormalize");

		for (int i = 0; i < m_row; i++)
			for (int j = i + 1; j < m_row; j++) {
				if (fabs(m_vec[i] * m_vec[j]) > 1e-4) return false;
			}
		return true;
	}

	void matrix::transpose() {
		double t;
		for (int i = 0; i < m_row; i++)
			for (int j = 0; j < i; j++) {
				t = m_vec[i][j];
				m_vec[i][j] = m_vec[j][i];
				m_vec[j][i] = t;
			}

	}

	void matrix::inverse() {
		double **a;

		a = new double*[m_row];
		for (int i = 0; i < m_row; i++) a[i] = new double[2 * m_col];

		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++) {
				a[i][j] = m_vec[i][j];
			}
		}

		for (int i = 0; i < m_row; i++) {
			for (int j = m_col; j < m_col * 2; j++)
				a[i][j] = (j - i == m_col) ? (1.0) : (0.0);
		}

		for (int i = 0; i < m_row; i++)
		{
			if (a[i][i] != 1.0)
			{
				double tmp = a[i][i];
				a[i][i] = 1.0;
				for (int j = i; j < m_col * 2; j++)
					a[i][j] /= tmp;
			}
			for (int k = 0; k < m_row; k++)
			{
				if (i != k)
				{
					double tmp = a[k][i];
					for (int j = 0; j < m_col * 2; j++)
						a[k][j] -= (tmp*a[i][j]);
				}
				else continue;
			}
		}

		for (int i = 0; i < m_row; i++) {
			for (int j = m_col; j < m_col * 2; j++)
				m_vec[i][j - m_col] = a[i][j];
		}

		for (int i = 0; i < m_row; i++) delete[] a[i];
		delete[] a;
		a = 0;
	}

	void matrix::freeMemory() {
		m_vec.clear();
	}
	void matrix::resize(const int row, const int col) {
		m_col = col; m_row = row;
		allocateMemory(row, col);
	}
	void matrix::allocateMemory(const int r, const int c) {

		m_vec.resize(r);
		for (int i = 0; i < r; i++)
			m_vec[i].m_data.resize(c);
	}

	myVector & matrix::operator[](const int idx) {
		return m_vec[idx];
	}

	const myVector & matrix::operator[](const int idx)const {
		return m_vec[idx];
	}
	void matrix::print() {
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++)
				cout << m_vec[i][j] << " ";
			cout << endl;
		}
	}

	void matrix::dalgonalize(double alpha) {
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++)
				if (i == j) m_vec[i][j] = pow(alpha, 0.5*(i / (m_col - 1)));
				else m_vec[i][j] = 0;
		}
	}
}