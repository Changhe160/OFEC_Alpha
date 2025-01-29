#include "vector.h"
#include "../../core/exception.h"
#include <algorithm>
#include <numeric>
#include "../functional.h"

namespace ofec {
	Vector::vector_::vector_(Vector &v, size_t idx) :m_vec(v), m_idx(idx) {

	}

	Vector::vector_ &Vector::vector_::operator=(const vector_ &rhs) {
		m_vec.m_data[m_idx] = rhs.m_vec.m_data[rhs.m_idx];
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_::operator Real()const {
		return m_vec.m_data[m_idx];
	}

	Vector::vector_ &Vector::vector_::operator=(Real val) {
		m_vec.m_data[m_idx] = val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ &Vector::vector_::operator+=(Real val) {
		m_vec.m_data[m_idx] += val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ &Vector::vector_::operator-=(Real val) {
		m_vec.m_data[m_idx] -= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ &Vector::vector_::operator*=(Real val) {
		m_vec.m_data[m_idx] *= val;
		m_vec.m_wrote = true;
		return *this;

	}

	Vector::vector_ &Vector::vector_::operator/=(Real val) {
		m_vec.m_data[m_idx] /= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::Vector(size_t size) : m_data(size) {}

	Vector::Vector(size_t size, Real val) : m_data(size, val) {}

	Vector::Vector(const std::vector<Real> &v) : m_data(v) {}

	Vector::Vector(std::vector<Real> &&v) : m_data(std::move(v)) {}

	Real& Vector::operator[](size_t idx) {
		return m_data[idx];
	}

	Real Vector::operator[](size_t idx) const {
		return m_data[idx];
	}

	Vector& Vector::operator=(const Vector &v) {
		if (this == &v) return *this;
		m_data = v.m_data;
		m_norm = v.m_norm;
		m_wrote = v.m_wrote;
		return *this;
	}

	Vector& Vector::operator=(Vector &&v) noexcept {
		m_data = std::move(v.m_data);
		m_norm = v.m_norm;
		m_wrote = v.m_wrote;
		return *this;
	}

	Vector& Vector::operator+=(const Vector &v) {
		if (m_data.size() != v.m_data.size()) throw Exception("the size of two vectors must be same by + operation@Vector::operator +=");
		std::transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::plus<Real>());
		m_wrote = true;
		return *this;
	}

	Vector& Vector::operator-=(const Vector &v) {
		if (m_data.size() != v.m_data.size()) throw Exception("the size of two vectors must be same by - operation@Vector::operator -=");
		std::transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::minus<Real>());
		m_wrote = true;
		return *this;
	}

	Vector Vector::projection(const Vector &v) const {
		Vector r(v);
		Real sv = 0;
		for (unsigned i = 0; i < m_data.size(); i++)
			sv += m_data[i] * v[i];
		sv /= v * v;
		r *= sv;
		return r;
	}

	void Vector::normalize() {
		if (norm() == 0) throw Exception("std::vector norm is zero @ Vector::normalize()");
		for (auto &i : m_data) {
			i /= norm();
		}
		m_norm = 1;
		m_wrote = false;
	}

	size_t Vector::size()const {
		return m_data.size();
	}

	void Vector::randomizeInSphere(Real radius, Uniform *rand) {
		Real r = rand->nextNonStd<Real>(0., radius);
		randomize(rand, -1, 1);
		normalize();
		for (auto &i : m_data) i *= r;
		m_wrote = true;

	}

	void Vector::randomizeOnSphere(Real radius, Uniform *rand) {
		randomize(rand, -1, 1);
		normalize();
		for (auto &i : m_data) i *= radius;
		m_norm = radius;
		m_wrote = false;
	}

	void Vector::randomizeOnSphere(Real radius, std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen) {
		randomize(unif, gen, -1, 1);
		normalize();
		for (auto &i : m_data) i *= radius;
		m_norm = radius;
		m_wrote = false;
	}

	void Vector::randomize(std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen, Real min, Real max) {
		for (auto &i : m_data) i = min + (unif(gen) - unif.min()) / (unif.max() - unif.min()) * (max - min);
		m_wrote = true;
	}

	void Vector::randomize(Uniform *uni, Real min, Real max) {
		for (auto &i : m_data) i = uni->nextNonStd(min, max);
		m_wrote = true;
	}

	void Vector::randomize(Normal *nor) {
		for (auto &i : m_data) i = nor->next();
		m_wrote = true;
	}

	void Vector::randomizeInSphere(Real radius, Normal *nor, Uniform *uni) {
		Real r = fabs(nor->nextNonStd(0, radius));
		randomize(uni, -1, 1);
		normalize();
		for (auto &i : m_data) i *= r;
		m_wrote = true;
	}

	Vector& Vector::operator=(const std::vector<Real> &v) {
		if (m_data.size() == v.size()) {
			m_data = v;
			m_wrote = true;
		}
		else {
			throw Exception("size not the same @ Vector::operator =(std::vector<Real>&)");
		}
		return *this;
	}

	Vector& Vector::operator*=(Real val) {
		for (auto &i : m_data) i *= val;
		m_wrote = true;
		return *this;
	}

	Vector& Vector::operator/=(Real val) {
		for (auto &i : m_data) i /= val;
		m_wrote = true;
		return *this;
	}

	Vector& Vector::operator-=(Real val) {
		for (auto &i : m_data) i -= val;
		m_wrote = true;
		return *this;
	}

	Vector& Vector::operator+=(Real val) {
		for (auto &i : m_data) i += val;
		m_wrote = true;
		return *this;
	}

	std::vector<Real>::iterator Vector::begin() {
		return m_data.begin();
	}

	std::vector<Real>::iterator Vector::end() {
		return m_data.end();
	}

	Vector operator-(const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = -i;
		return Vector(std::move(v));
	}

	Vector operator*(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i *= val;
		return Vector(std::move(v));
	}
	
	Vector operator/(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i /= val;
		return Vector(std::move(v));
	}

	Vector operator+(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i += val;
		return Vector(std::move(v));
	}

	Vector operator-(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i -= val;
		return Vector(std::move(v));
	}

	Vector operator>(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val > vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator<(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val < vec.m_data[i];
		return Vector(std::move(v));
	}
	
	Vector operator>=(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val >= vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator<=(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val <= vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator==(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val == vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator!=(Real val, const Vector &vec) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = val != vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator*(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i *= val;
		return Vector(std::move(v));
	}

	Vector operator/(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i /= val;
		return Vector(std::move(v));
	}

	Vector operator+(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i += val;
		return Vector(std::move(v));
	}

	Vector operator-(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i -= val;
		return Vector(v);
	}

	Vector operator>(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] > val;
		return Vector(std::move(v));
	}

	Vector operator<(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] < val;
		return Vector(std::move(v));
	}

	Vector operator>=(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] >= val;
		return Vector(std::move(v));
	}

	Vector operator<=(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] <= val;
		return Vector(std::move(v));
	}

	Vector operator==(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] == val;
		return Vector(std::move(v));
	}

	Vector operator!=(const Vector &vec, Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto &i : v) i = vec.m_data[i] != val;
		return Vector(std::move(v));
	}

	Real operator*(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size()) 
			throw Exception("the size of two vectors must be same by * operation");
		Real sum = 0;
		for (unsigned i = 0; i < left_vec.m_data.size(); i++) sum += left_vec.m_data[i] * right_vec.m_data[i];
		return sum;
	}

	Vector operator+(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size()) 
			throw Exception("the size of two vectors must be same by + operation");
		Vector r(left_vec);
		return r += right_vec;
	}

	Vector operator-(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size()) 
			throw Exception("the size of two vectors must be same by - operation");
		Vector r(left_vec);
		return r -= right_vec;
	}

	Vector operator>(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by > operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] > right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator<(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by < operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] < right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator>=(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by >= operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] >= right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator<=(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by <= operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] <= right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator==(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by == operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] == right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector operator!=(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by != operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < left_vec.m_data.size(); ++i)
			v[i] = left_vec.m_data[i] != right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Real norm(const Vector &vec, Real p) {
		Real s = 0;
		for (auto &i : vec.m_data)
			s += pow(abs(i), p);
		return pow(s, 1.0 / p);
	}

	Real sum(const Vector &vec) {
		Real s = 0;
		for (auto &i : vec.m_data)
			s += i;
		return s;
	}

	Real prod(const Vector &vec) {
		Real p = 1;
		for (auto &i : vec.m_data)
			p *= i;
		return p;
	}

	Real min(const Vector &vec) {
		return *std::min_element(vec.m_data.begin(), vec.m_data.end());
	}

	Real max(const Vector &vec) {
		return *std::max_element(vec.m_data.begin(), vec.m_data.end());
	}

	Real mean(const Vector &vec) {
		return sum(vec) / vec.m_data.size();
	}

	/*-----------------------------------------------------------------------------------------------
	* https://www.mathworks.com/help/matlab/ref/prctile.html
	* -----------------------------------------------------------------------------------------------
	* For an n-element vector A, prctile returns percentiles by using a sorting-based algorithm:
	*	1. The sorted elements in A are taken as the 100(0.5/n)th, 100(1.5/n)th, ..., 100([n ¨C 0.5]/n)th percentiles. For example:
	*		- For a data vector of five elements such as {6, 3, 2, 10, 1}, the sorted elements {1, 2, 3, 6, 10} respectively correspond to the 10th, 30th, 50th, 70th, and 90th percentiles.
	*		- For a data vector of six elements such as {6, 3, 2, 10, 8, 1}, the sorted elements {1, 2, 3, 6, 8, 10} respectively correspond to the (50/6)th, (150/6)th, (250/6)th, (350/6)th, (450/6)th, and (550/6)th percentiles.
	*	2. prctile uses linear interpolation to compute percentiles for percentages between 100(0.5/n) and 100([n ¨C 0.5]/n).
	*	3. prctile assigns the minimum or maximum values of the elements in A to the percentiles corresponding to the percentages outside that range.
	* prctile treats NaNs as missing values and removes them.
	*------------------------------------------------------------------------------------------------*/
	Real prctile(const Vector &vec, size_t p) {
		size_t n = vec.m_data.size();
		if (n == 0)
			throw Exception("Vector must not be empty@ Vector::prctile");
		else if (n == 1)
			return vec.m_data.front();
		else {
			auto s = vec.m_data;
			std::sort(s.begin(), s.end());
			std::vector<Real> q(n);
			for (size_t i = 0; i < n; ++i)
				q[i] = (i + 0.5) / n;
			Real q_p = p / 100.0;
			if (q_p < q.front())
				return s.front();
			else if (q_p > q.back())
				return s.back();
			else {
				size_t l = floor(q_p * n), r = l + 1;
				return s[l] + (q_p - q[l]) / (q[r] - q[l]) * (s[r] - s[l]);
			}
		}
	}

	Real median(const Vector &vec) {
		return prctile(vec, 50);
	}

	Real maxDiff(const Vector &vec) {
		return max(vec) - min(vec);
	}

	Vector power(const Vector &vec, Real exp) {
		std::vector<Real> v(vec.m_data.size());
		for (size_t i = 0; i < vec.m_data.size(); ++i)
			v[i] = pow(vec.m_data[i], exp);
		return Vector(std::move(v));
	}

	Vector squareRoot(const Vector &vec) {
		std::vector<Real> v(vec.m_data.size());
		for (size_t i = 0; i < vec.m_data.size(); ++i)
			v[i] = sqrt(vec.m_data[i]);
		return Vector(std::move(v));
	}

	Vector logarithm(const Vector &vec) {
		std::vector<Real> v(vec.m_data.size());
		for (size_t i = 0; i < vec.m_data.size(); ++i)
			v[i] = log(vec.m_data[i]);
		return Vector(std::move(v));
	}

	Vector absolute(const Vector &vec) {
		std::vector<Real> v(vec.m_data.size());
		for (size_t i = 0; i < vec.m_data.size(); ++i)
			v[i] = abs(vec.m_data[i]);
		return Vector(std::move(v));
	}

	Vector normcdf(const Vector &vec) {
		std::vector<Real> v(vec.m_data.size());
		for (size_t i = 0; i < vec.m_data.size(); ++i)
			v[i] = normCDF(vec.m_data[i]);
		return Vector(std::move(v));
	}

	std::vector<int> find(const Vector &vec) {
		std::vector<int> v;
		for (int i = 0; i < vec.m_data.size(); ++i)
			if (vec.m_data[i])	v.push_back(i);
		return v;
	}

	std::vector<int> sort(const Vector &vec, bool ascend) {
		std::vector<int> v(vec.m_data.size());
		std::iota(v.begin(), v.end(), 0);
		if (ascend)
			std::sort(v.begin(), v.end(), [&vec](int i, int j) {return vec.m_data[i] < vec.m_data[j]; });
		else
			std::sort(v.begin(), v.end(), [&vec](int i, int j) {return vec.m_data[i] > vec.m_data[j]; });
		return v;
	}

	Vector min(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by min operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = left_vec.m_data[i] < right_vec.m_data[i] ? left_vec.m_data[i] : right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector max(const Vector &left_vec, const Vector &right_vec) {
		if (left_vec.m_data.size() != right_vec.m_data.size())
			throw Exception("the size of two vectors must be same by max operation");
		std::vector<Real> v(left_vec.m_data.size());
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = left_vec.m_data[i] > right_vec.m_data[i] ? left_vec.m_data[i] : right_vec.m_data[i];
		return Vector(std::move(v));
	}

	Real Vector::angle(Vector &v) {
		return acos((*this) * (v) / (norm() * v.norm()));
	}

	void Vector::pushBack(Real n) {
		m_data.push_back(n);
		m_wrote = true;
	}

	void Vector::clear() {
		m_data.clear();
		m_wrote = true;
	}

	Real Vector::distance(const Vector &point) const {
		if (point.size() != size()) throw Exception("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= point;
		return vec.norm();
	}

	Real Vector::distance(const std::vector<Real> &point) const {
		if (point.size() != size()) throw Exception("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= point;
		return vec.norm();
	}

	Real Vector::norm() {
		if (m_wrote) {
			norm_();
			m_wrote = false;
		}
		return m_norm;
	}

	void Vector::zeroize() {
		for (auto &i : m_data) 		i = 0;
		m_norm = 0;
		m_wrote = false;
	}

	Vector Vector::pointBetween(const Vector &v1, Real r) const {
		std::vector<Real> v(m_data);
		for (auto i = 0; i < m_data.size(); ++i) {
			v[i] = m_data[i] * r + v1[i] * (1 - r);
		}
		return Vector(std::move(v));
	}

	void  Vector::resize(size_t n) noexcept {
		m_data.resize(n);
		m_wrote = true;
	}

	void Vector::norm_() {
		m_norm = 0;
		for (auto &i : m_data) m_norm += i * i;
		m_norm = std::sqrt(m_norm);
	}

	Real Vector::perpendicularDistance(const Vector &point) {
		Real numerator = 0, denominator = 0;
		for (size_t i = 0; i < m_data.size(); ++i) {
			numerator += m_data[i] * point[i];
			denominator += m_data[i] * m_data[i];
		}
		Real k = numerator / denominator;

		Real d = 0;
		for (size_t i = 0; i < m_data.size(); i += 1)
			d += (k * m_data[i] - point[i]) * (k * m_data[i] - point[i]);

		return sqrt(d);
	}

	Vector Vector::times(const Vector &vec) const {
		if (m_data.size() != vec.m_data.size()) 
			throw Exception("the size of two vectors must be same by times operation@Vector::times");
		std::vector<Real> v(m_data.size());
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = m_data[i] * vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector Vector::rdivide(const Vector &vec) const {
		if (m_data.size() != vec.m_data.size())
			throw Exception("the size of two vectors must be same by rdivide operation@Vector::rdivide");
		std::vector<Real> v(m_data.size());
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = m_data[i] / vec.m_data[i];
		return Vector(std::move(v));
	}

	Vector Vector::concat(const Vector &vec) const {
		std::vector<Real> v(m_data.size() + vec.m_data.size());
		std::copy(m_data.begin(), m_data.end(), v.begin());
		std::copy(vec.m_data.begin(), vec.m_data.end(), v.begin() + m_data.size());
		return Vector(std::move(v));
	}

	Vector Vector::concat(Real val) const {
		std::vector<Real> v(m_data.size() + 1);
		std::copy(m_data.begin(), m_data.end(), v.begin());
		v.back() = val;
		return Vector(std::move(v));
	}

	Vector Vector::slice(const std::vector<int> &ids) const {
		std::vector<Real> v(ids.size());
		for (size_t i = 0; i < ids.size(); ++i)
			v[i] = ids[i] < 0 ? m_data[ids.size() - ids[i]] : m_data[ids[i]];
		return Vector(std::move(v));
	}

	Vector Vector::power(Real exp) const {
		std::vector<Real> v(m_data.size());
		for (size_t i = 0; i < m_data.size(); ++i)
			v[i] = pow(m_data[i], exp);
		return Vector(std::move(v));
	}

	Vector Vector::zeros(size_t n) {
		return Vector(n, 0);
	}

	Vector Vector::rand(size_t n, Uniform *unif) {
		std::vector<Real> v(n);
		for (size_t i = 0; i < n; ++i)
			v[i] = unif->next();
		return Vector(std::move(v));
	}

	Vector Vector::randn(size_t n, Normal *norm) {
		std::vector<Real> v(n);
		for (size_t i = 0; i < n; ++i)
			v[i] = norm->next();
		return Vector(std::move(v));
	}

	std::ifstream& operator>>(std::ifstream &in, Vector &v) {
		for (auto &i : v.m_data) {
			in >> i;
		}
		v.m_wrote = true;
		return in;
	}

	std::ostream& operator<<(std::ostream &out, Vector &v) {
		for (auto &i : v.m_data)
			out << i << ' ';
		return out;
	}
}