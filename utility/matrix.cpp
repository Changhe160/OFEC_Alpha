#include "matrix.h"
#include "myexcept.h"

namespace OFEC {

	matrix::matrix(int dim) :m_col_size(dim), m_row_size(dim) {
		if (m_row_size == 0) return;
		resize(m_row_size, m_col_size);

	}
	matrix::matrix(int c, int r) : m_col_size(c), m_row_size(r) {
		resize(m_row_size, m_col_size);
	}
	void matrix::set_row(const double *d, const int c, const int r) {
		if (r != m_row_size) return;
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<m_col_size; j++)
				m_row[i][j] = d[j];
	}
	void matrix::set_col(const double *d, const int r, const int c) {
		if (c != m_col_size) return;
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<m_col_size; j++)
				m_row[i][j] = d[i];

	}
	void matrix::set(const double * const * d) {
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<m_col_size; j++)
				m_row[i][j] = d[i][j];
	}

	matrix & matrix::operator *=(const matrix &m) {
		//TODO: GPU computing
		if (m_col_size != m.m_row_size) throw myexcept("can not *, m_col_size of 1st matrix must be equal to m_row_size of 2nd matrix@ matrix::operator *=");

		matrix r(m.m_col_size, m_row_size);
		for (int i = 0; i<m_row_size; i++) {
			for (int j = 0; j<m.m_col_size; j++) {
				r.m_row[i][j] = 0;
				for (int k = 0; k<m_col_size; k++)
					r.m_row[i][j] += m_row[i][k] * m.m_row[k][j];
			}
		}
		if (m_row_size != m.m_row_size || m_col_size != m.m_col_size) {
			clear();
			resize(r.m_row_size, r.m_col_size);
		}
		copy(r.m_row.begin(), r.m_row.end(), this->m_row.begin());

		return *this;
	}
	matrix & matrix::operator *=(double x) {
		for (int i = 0; i<m_row_size; i++) {
			m_row[i] *= x;
		}
		return *this;
	}

	matrix&matrix::operator =(const matrix &m) {
		if (m_row_size != m.m_row_size || m_col_size != m.m_col_size) {
			throw myexcept("Assiment operator matrix@matrix::operator =");
		}
		if (this == &m) return *this;
		m_row = m.m_row;
		return *this;
	}
	bool matrix::identify() {
		if (m_row_size != m_col_size) return false;
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<m_col_size; j++)
				m_row[i][j] = (j == i);

		return true;
	}
	bool matrix::is_identity() {
		if (m_row_size != m_col_size) return false;
		for (int i = 0; i<m_row_size; i++) {
			for (int j = 0; j<m_col_size; j++) {
				if (m_row[i][j] != (i == j ? 1. : 0)) {
					return false;
				}
			}
		}
		return true;
	}
	void matrix::diagonalize(normal * rand, double CondiNum) {
		if (m_row_size != m_col_size) throw myexcept("can not be diagonal, matrix must be squre@matrix::diagonalize");

		double min, max;
		double *r = new double[m_row_size];
		for (int i = 0; i < m_row_size; i++)	r[i] = rand->next_non_standard(1., m_row_size*1.0);

		min = max = r[0];
		for (int i = 0; i < m_row_size; i++) {
			if (min > r[i])min = r[i];
			if (max < r[i]) max = r[i];
		}
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++)
				if (j != i) m_row[i][j] = 0.;
				else m_row[i][j] = pow(CondiNum, (r[i] - min) / (max - min));
		}
		delete[]r;

	}
	void matrix::zero() {
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<m_col_size; j++)
				m_row[i][j] = 0.;
	}
	void matrix::set_rotation_axes(int r, int c, double angle) {
		identify();
		m_row[r][r] = cos(angle);
		m_row[r][c] = -sin(angle);
		m_row[c][r] = sin(angle);
		m_row[c][c] = cos(angle);
	}


	void matrix::generate_rotation(normal *rand, double CondiNum) {
		/*P. N. Suganthan, N. Hansen, J. J. Liang, K. Deb, Y.-P. Chen, A. Auger and S. Tiwari,
		"Problem Definitions and Evaluation Criteria for the CEC 2005 Special Session on Real-Parameter
		Optimization", Technical Report, Nanyang Technological University, Singapore, May 2005 AND KanGAL
		Report #2005005, IIT Kanpur, India.*/

		matrix ortholeft(m_row_size), orthoright(m_row_size), diagonal(m_row_size);
		ortholeft.randomize(rand);
		ortholeft.orthonormalize();
		orthoright.randomize(rand);
		orthoright.orthonormalize();
		diagonal.diagonalize(rand, CondiNum);
		ortholeft *= diagonal;
		ortholeft *= orthoright;
		*this = ortholeft;
	}

	void matrix::randomize(uniform *rand) {
		for (int i = 0; i<m_row_size; i++)
			m_row[i].randomize(rand, -1, 1);
	}
	void matrix::randomize(normal *rand) {
		for (int i = 0; i<m_row_size; i++)
			m_row[i].randomize(rand);
	}

	void matrix::orthonormalize() {
		if (m_row_size != m_col_size) throw myexcept("cannot perform orthonormalization, matrix must be squre@matrix::orthonormalize");
		//if (!(is_inverse())) throw myexcept("cannot perform function orthonormalize(), matrix must be reversible@matrix::orthonormalize");
		Vector v(m_col_size), q(m_col_size);
		double r = 0;
		//modified Gram schmidt process
		for (int i = 0; i<m_row_size; i++) {
			v = m_row[i];
			q = m_row[i];
			for (int j = 0; j<i; j++) {
				r = v * m_row[j];
				v -= m_row[j] * r;
			}
			r = v.length();
			m_row[i] = v / r;
		}
		// normalize each vector
		for (int i = 0; i < m_row_size; i++) m_row[i].normalize();
	}
	bool matrix::is_orthogonal() {
		/*if (m_row_size != m_col_size) throw myexcept("cannot perform function is_orthogonal(), matrix must be squre@matrix::orthonormalize");
		matrix temp1 = *this;
		temp1.transpose();
		const matrix& temp2 = temp1;
		temp1 *= temp2;
		return temp1.is_identity();*/
		if (m_row_size != m_col_size) throw myexcept("cannot perform function is_orthogonal(), matrix must be squre@matrix::is_orthogonal");

		for (int i = 0; i < m_row_size; i++) {
			for (int j = i + 1; j < m_row_size; j++) {
				if (fabs(m_row[i] * m_row[j]) > 1e-4) return false;
			}
			if (m_row[i].length()!=1.) return false;
		}
		return true;
	}
	void matrix::transpose() {
		double t;
		for (int i = 0; i<m_row_size; i++)
			for (int j = 0; j<i; j++) {
				t = m_row[i][j];
				m_row[i][j] = m_row[j][i];
				m_row[j][i] = t;
			}

	}

	void matrix::inverse() {
		if(!(is_inverse())) throw myexcept("cannot perform function inverse(), matrix must be reversible@matrix::inverse");
		double **a;

		a = new double*[m_row_size];
		for (int i = 0; i<m_row_size; i++) a[i] = new double[2 * m_col_size];

		for (int i = 0; i<m_row_size; i++) {
			for (int j = 0; j<m_col_size; j++) {
				a[i][j] = m_row[i][j];
			}
		}

		for (int i = 0; i<m_row_size; i++) {
			for (int j = m_col_size; j<m_col_size * 2; j++)
				a[i][j] = (j - i == m_col_size) ? (1.0) : (0.0);
		}

		for (int i = 0; i<m_row_size; i++)
		{
			if (a[i][i] != 1.0)
			{
				double tmp = a[i][i];
				a[i][i] = 1.0;
				for (int j = i; j<m_col_size * 2; j++)
					a[i][j] /= tmp;
			}
			for (int k = 0; k<m_row_size; k++)
			{
				if (i != k)
				{
					double tmp = a[k][i];
					for (int j = 0; j<m_col_size * 2; j++)
						a[k][j] -= (tmp*a[i][j]);
				}
				else continue;
			}
		}

		for (int i = 0; i<m_row_size; i++) {
			for (int j = m_col_size; j<m_col_size * 2; j++)
				m_row[i][j - m_col_size] = a[i][j];
		}

		for (int i = 0; i<m_row_size; i++) delete[] a[i];
		delete[] a;

	}

	std::vector<Vector>& matrix::data() {
		return m_row;
	}

	bool matrix::is_inverse() {
		return !(compute_determinant(m_row,m_col_size)==0.);
	}

	double matrix::compute_determinant(std::vector<Vector>& temp, int num ) {

		int i = 0, j = 0, c = 0; /*i，j为行与列,c为数组b的行*/
		std::vector<Vector> temp_b(m_row_size, Vector(m_col_size, 0.)); /*定义向量b并初始化*/
		int p = 0, q = 0;/*p,q为中间变量*/
		int sum = 0;/*sum为行列式的值*/

		if (num == 1)
			return temp[0][0];
		for (i = 0; i<num; i++)/*此处大循环实现将余子式存入数组b中*/
		{
			for (c = 0; c<num - 1; c++)
			{
				if (c<i)    /*借助c判断每行的移动方法*/
					p = 0;

				else
					p = 1;

				for (j = 0; j<num - 1; j++)
				{
					temp_b[c][j] = temp[c + p][j + 1];
				}
			}
			if (i % 2 == 0)  q = 1;
			else  q = (-1);

			sum = sum + temp[i][0] * q*compute_determinant(temp_b, num - 1);
		}
		return sum;
		
	}
	void matrix::clear() {
		m_row.clear();
	}
	void matrix::resize(int row, int col) {
		m_col_size = col;
		m_row_size = row;
		m_row.resize(row);
		for (int i = 0; i<row; i++)
			m_row[i].resize(col);

	}

	Vector & matrix::operator[](const int idx) {
		return m_row[idx];
	}

	const Vector & matrix::operator[](const int idx)const {
		return m_row[idx];
	}


	void matrix::diagonalize(double alpha) {
		for (int i = 0; i<m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++)
				if (i == j) m_row[i][j] = pow(alpha, 0.5*(i / (m_col_size - 1)));
				else m_row[i][j] = 0;
		}
	}

	void matrix::load(std::ifstream &in) {
		for (int i = 0; i<m_row_size; i++) {
			in >> m_row[i];
		}
	};
	void matrix::print(std::ofstream & out) {
		for (int i = 0; i<m_row_size; i++) {
			for (int j = 0; j<m_col_size; j++)
				out << m_row[i][j] << " ";
			out << std::endl;
		};
	}
}