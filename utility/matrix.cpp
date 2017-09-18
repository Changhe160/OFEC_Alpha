#include "matrix.h"
#include "myexcept.h"

namespace OFEC {

	matrix::matrix(int dim) :m_col(dim), m_row(dim) {
		if (m_row == 0) return;
		resize(m_row, m_col);

	}
	matrix::matrix( int c, int r) : m_col(c), m_row(r) {
		resize(m_row, m_col);
	}
	void matrix::set_row(const double *d, const int c, const int r) {
		if (r != m_row) return;
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<m_col; j++)
				m_vec[i][j] = d[j];
	}
	void matrix::set_col(const double *d, const int r, const int c) {
		if (c != m_col) return;
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<m_col; j++)
				m_vec[i][j] = d[i];

	}
	void matrix::set(const double * const * d) {
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<m_col; j++)
				m_vec[i][j] = d[i][j];
	}

	matrix & matrix::operator *=(const matrix &m) {
		//TODO: GPU computing
		if (m_col != m.m_row) throw myexcept("can not *, m_col of 1st matrix must be equal to m_row of 2nd matrix@ matrix::operator *=");

		matrix r(m.m_col, m_row);
		for (int i = 0; i<m_row; i++) {
			for (int j = 0; j<m.m_col; j++) {
				r.m_vec[i][j] = 0;
				for (int k = 0; k<m_col; k++)
					r.m_vec[i][j] += m_vec[i][k] * m.m_vec[k][j];
			}
		}
		if (m_row != m.m_row || m_col != m.m_col) {
			clear();
			resize(r.m_row, r.m_col);
		}
		copy(r.m_vec.begin(), r.m_vec.end(), this->m_vec.begin());

		return *this;
	}
	matrix & matrix::operator *=( double x) {
		for (int i = 0; i<m_row; i++) {
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
	bool matrix::identify() {
		if (m_row != m_col) return false;
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<m_col; j++)
				m_vec[i][j] = (j == i);

		return true;
	}
	bool matrix::is_identity() {
		if (m_row != m_col) return false;
		for (int i = 0; i<m_row; i++) {
			for (int j = 0; j<m_col; j++) {
				if (m_vec[i][j] != (i == j ? 1. : 0)) {
					return false;
				}
			}
		}
		return true;
	}
	void matrix::diagonalize(uniform * rand, double CondiNum) {
		if (m_row != m_col) throw myexcept("can not be diagonal, matrix must be squre@matrix::diagonalize");

		double min, max;
		double *r = new double[m_row];
		for (int i = 0; i < m_row; i++)	r[i] = rand->next_non_standard(1., m_row*1.0);

		min = max = r[0];
		for (int i = 0; i < m_row; i++) {
			if (min > r[i])min = r[i];
			if (max < r[i]) max = r[i];
		}
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++)
				if (j != i) m_vec[i][j] = 0.;
				else m_vec[i][j] = pow(CondiNum, (r[i] - min) / (max - min));				
		}
		delete[]r;

	}
	void matrix::zero() {
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<m_col; j++)
				m_vec[i][j] = 0.;
	}
	void matrix::set_rotation_axes(int r, int c, double angle) {
		identify();
		m_vec[r][r] = cos(angle);
		m_vec[r][c] = -sin(angle);
		m_vec[c][r] = sin(angle);
		m_vec[c][c] = cos(angle);
	}


	void matrix::generate_rotation(uniform *rand, double CondiNum) {
		/*P. N. Suganthan, N. Hansen, J. J. Liang, K. Deb, Y.-P. Chen, A. Auger and S. Tiwari,
		"Problem Definitions and Evaluation Criteria for the CEC 2005 Special Session on Real-Parameter
		Optimization", Technical Report, Nanyang Technological University, Singapore, May 2005 AND KanGAL 
		Report #2005005, IIT Kanpur, India.*/

		matrix ortholeft(m_row), orthoright(m_row), diagonal(m_row);
		ortholeft.randomize(rand);
		ortholeft.orthonormalize();
		orthoright.randomize(rand);
		orthoright.orthonormalize();
		diagonal.diagonalize(rand,CondiNum);
		ortholeft *= diagonal;
		ortholeft *= orthoright;
		*this = ortholeft;
	}

	void matrix::randomize(uniform *rand) {
		for (int i = 0; i<m_row; i++) 
			m_vec[i].randomize(rand,-1, 1);
	}
	void matrix::randomize(normal *rand) {
		for (int i = 0; i<m_row; i++)
			m_vec[i].randomize(rand);
	}

	void matrix::orthonormalize() {
		if (m_row != m_col) throw myexcept("cannot perform orthonormalization, matrix must be squre@matrix::orthonormalize");

		Vector v(m_col), q(m_col);
		double r = 0;
		//modified Gram schmidt process
		for (int i = 0; i<m_row; i++) {
			v = m_vec[i];
			q = m_vec[i];
			for (int j = 0; j<i; j++) {
				r = v * m_vec[j];
				v -= m_vec[j] * r;
			}
			r = v.length();
			m_vec[i] = v / r;
		}
		// normalize each vector
		for (int i = 0; i < m_row; i++) m_vec[i].normalize();
	}
	void matrix::transpose() {
		double t;
		for (int i = 0; i<m_row; i++)
			for (int j = 0; j<i; j++) {
				t = m_vec[i][j];
				m_vec[i][j] = m_vec[j][i];
				m_vec[j][i] = t;
			}

	}

	void matrix::inverse() {
		double **a;

		a = new double*[m_row];
		for (int i = 0; i<m_row; i++) a[i] = new double[2 * m_col];

		for (int i = 0; i<m_row; i++) {
			for (int j = 0; j<m_col; j++) {
				a[i][j] = m_vec[i][j];
			}
		}

		for (int i = 0; i<m_row; i++) {
			for (int j = m_col; j<m_col * 2; j++)
				a[i][j] = (j - i == m_col) ? (1.0) : (0.0);
		}

		for (int i = 0; i<m_row; i++)
		{
			if (a[i][i] != 1.0)
			{
				double tmp = a[i][i];
				a[i][i] = 1.0;
				for (int j = i; j<m_col * 2; j++)
					a[i][j] /= tmp;
			}
			for (int k = 0; k<m_row; k++)
			{
				if (i != k)
				{
					double tmp = a[k][i];
					for (int j = 0; j<m_col * 2; j++)
						a[k][j] -= (tmp*a[i][j]);
				}
				else continue;
			}
		}

		for (int i = 0; i<m_row; i++) {
			for (int j = m_col; j<m_col * 2; j++)
				m_vec[i][j - m_col] = a[i][j];
		}

		for (int i = 0; i<m_row; i++) delete[] a[i];
		delete[] a;
	
	}

	void matrix::clear() {
		m_vec.clear();
	}
	void matrix::resize( int row,  int col) {
		m_col = col; 
		m_row = row;
		m_vec.resize(row);
		for (int i = 0; i<row; i++)
			m_vec[i].resize(col);

	}

	Vector & matrix::operator[](const int idx) {
		return m_vec[idx];
	}

	const Vector & matrix::operator[](const int idx)const {
		return m_vec[idx];
	}


	void matrix::diagonalize(double alpha) {
		for (int i = 0; i<m_row; i++) {
			for (int j = 0; j < m_col; j++)
				if (i == j) m_vec[i][j] = pow(alpha, 0.5*(i / (m_col - 1)));
				else m_vec[i][j] = 0;
		}
	}

	void matrix::load(std::ifstream &in) {
		for (int i = 0; i<m_row; i++) {
			in >> m_vec[i];
		}
	};
}

