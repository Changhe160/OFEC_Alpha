#include "vector.h"
#include "myexcept.h"

namespace OFEC {

	Vector::vector_::vector_(Vector& v, int idx) :m_vec(v), m_idx(idx) {

	}
	Vector::vector_& Vector::vector_::operator=(const vector_& rhs) {
		m_vec.m_data[m_idx] = rhs.m_vec.m_data[rhs.m_idx];
		m_vec.m_wrote = true;
		return *this;
	}
	Vector::vector_::operator real()const {
		return m_vec.m_data[m_idx];
	}
	Vector::vector_&  Vector::vector_::operator=(double val) {
		m_vec.m_data[m_idx] = val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator+=(double val) {
		m_vec.m_data[m_idx] += val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator-=(double val) {
		m_vec.m_data[m_idx] -= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator*=(double val) {
		m_vec.m_data[m_idx] *= val;
		m_vec.m_wrote = true;
		return *this;
		
	}
	Vector::vector_ & Vector::vector_::operator/=(double val) {
		m_vec.m_data[m_idx] /= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::Vector(int size, real *val) :m_data(size) {
		copy(val, val + size, m_data.begin());
	}
	Vector::Vector(int size) : m_data(size) {

	}
	Vector::Vector(int size, real val) : m_data(size, val){

	}
	Vector::Vector(const std::vector<real> & v) : m_data(v) {

	}
	Vector::Vector(const std::vector<real> &&v) : m_data(std::move(v)) {
	
	}

	Vector::vector_ Vector::operator [](int idx) {
		return Vector::vector_(*this, idx);
	}
	const double & Vector::operator [](int idx)const {
		return m_data[idx];
	}
	Vector &Vector::operator =(const Vector & v) {
		if (this == &v) return *this;
		m_data = v.m_data;
		m_length = v.m_length;
		m_wrote = v.m_wrote;
		return *this;
	}
	Vector &  Vector::operator +=(const Vector & v) {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by + operation@Vector::operator +=");
		transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::plus<double>());
		m_wrote = true;
		return *this;
	}
	Vector &  Vector::operator -=(const Vector & v) {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by - operation@Vector::operator -=");
		transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::minus<double>());
		m_wrote = true;
		return *this;
	}
	double Vector::operator *(const Vector & v) const {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by * operation@Vector::operator *");
		double sum = 0;
		for (unsigned i = 0; i < m_data.size(); i++) sum += m_data[i] * v.m_data[i];
		return sum;
	}

	Vector Vector::projection( const Vector &v) const{
		Vector r(v);
		double sv = 0;
		for (unsigned i = 0; i < m_data.size(); i++) 
			sv += m_data[i] * v[i];
		
		sv/= v*v;
		r *= sv;
		return r;
	}

	void Vector::normalize() {
		if (length() == 0) throw myexcept("std::vector length is zero @ Vector::normalize()");
		for (auto &i : m_data) {
			i /= length();
		}
		m_length = 1;
		m_wrote = false;
	}

	size_t Vector::size()const {
		return m_data.size();
	}
	void Vector::randomize_in_sphere(double radius, uniform * rand) {
		
		double r = rand->next_non_standard(0., radius);
		randomize(rand,-1, 1);
		normalize();
		for (auto&i : m_data) i *= r;
		m_wrote = true;
		
	}
	void Vector::randomize_on_sphere(double radius, uniform * rand) {
		
		randomize(rand,-1, 1);
		normalize();
		for (auto&i : m_data) i *= radius;
		m_length = radius;
		m_wrote = false;
		
	}

	void Vector::randomize_on_sphere(double radius, std::uniform_real_distribution<real> &unif, std::default_random_engine &gen) {

		randomize(unif, gen, -1, 1);
		normalize();
		for (auto&i : m_data) i *= radius;
		m_length = radius;
		m_wrote = false;

	}
	void Vector::randomize(std::uniform_real_distribution<real> &unif, std::default_random_engine &gen, real min, real max) {
		for (auto&i : m_data) i = min + (unif(gen) - unif.min()) / (unif.max() - unif.min())*(max - min);
		m_wrote = true;
	}
	void Vector::randomize(uniform * rand,real min, real max) {
		for (auto&i : m_data) i = rand->next_non_standard(min, max);
		m_wrote = true;		
	}
	void Vector::randomize(normal *rand) {
		
		for (auto&i : m_data) i = rand->next();
		m_wrote = true;
	}
	void Vector::randomize_in_sphere(double radius, normal *nor, uniform *uni) {
	
		double r = fabs(nor->next_non_standard(0, radius));
		randomize(uni,-1, 1);
		normalize();
		for (auto&i : m_data) i *= r;
		m_wrote = true;
	
	}
	Vector &Vector::operator =(const std::vector<real> & v) {
		if (m_data.size() == v.size()) {
			m_data = v;
			m_wrote = true;
		}
		else {
			throw myexcept("size not the same @ Vector::operator =(std::vector<real>&)");
		}
		return *this;
	}

	Vector & Vector::operator *=(real val) {
		for (auto &i : m_data) i *= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator /=(real val) {
		for (auto &i : m_data) i /= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator -=(real val) {
		for (auto &i : m_data) i -= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator +=(real val) {
		for (auto &i : m_data) i += val;
		m_wrote = true;
		return *this;
	}
	std::vector<real>::iterator  Vector::begin() {
		return m_data.begin();
	}
	std::vector<real>::iterator  Vector::end() {
		return m_data.end();
	}
	Vector  Vector::operator *(real val)const {
		std::vector<real> v(m_data);
		for (auto&i : v) i *= val;
		return Vector(v);
	}

	Vector  Vector::operator /(real val) const {
		std::vector<real> v(m_data);
		for (auto&i : v) i /= val;
		return Vector(v);
	}
	Vector  Vector::operator -(real val) const {
		std::vector<real> v(m_data);
		for (auto&i : v) i -= val;
		return Vector(v);
	}
	Vector  Vector::operator +(real val)const {
		std::vector<real> v(m_data);
		for (auto&i : v) i += val;
		return Vector(v);
	}
	double Vector::angle( Vector & v){		
		return acos(this->operator*(v) / (length()*v.length()));
	}

	void Vector::push_back(real n) {
		m_data.push_back(n);
		m_wrote = true;
	}

	double Vector::distance(const Vector&v) const {
		if (v.size() != size()) throw myexcept("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= v;
		return vec.length();
	}
	double Vector::distance(const std::vector<real>&v) const {
		if (v.size() != size()) throw myexcept("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= v;
		return vec.length();
	}

	std::ifstream &operator>>(std::ifstream & in, Vector&v) {
		for (auto &i : v.m_data) {
			in >> i;
		}
		v.m_wrote = true;
		return in;
	}
	double Vector::length() {
		if (m_wrote) {
			length_();
			m_wrote = false;
		}
		return m_length;
	}
	Vector::Vector(Vector&& rhs) :m_data(move(rhs.m_data)), m_length(rhs.m_length), m_wrote(rhs.m_wrote) {

	}

	Vector& Vector::operator=(Vector&& rhs) {
		m_data = std::move(rhs.m_data);
		m_length = rhs.m_length;
		m_wrote = rhs.m_wrote;

		return *this;
	}

	void Vector::zero() {
		for (auto &i : m_data) 		i = 0;
		m_length = 0;
		m_wrote = false;
	}

	Vector Vector::point_between(const Vector & v1, double r) const{
		std::vector<real> v(m_data);
		for (auto i = 0; i < m_data.size(); ++i) {
			v[i] = m_data[i] * r + v1[i] * (1 - r);
		}
		return Vector(v);
	}

	void  Vector::resize(int n) noexcept {
		m_data.resize(n);
		m_wrote = true;
	}

	void Vector::length_() {
		m_length = 0;
		for (auto &i : m_data) m_length += i*i;
		m_length = std::sqrt(m_length);
	}

	double perpendicular_distance(const Vector &direction, const Vector &point) {
		double numerator = 0, denominator = 0;
		for (size_t i = 0; i<direction.size(); i += 1){
			numerator += direction[i] * point[i];
			denominator += direction[i] * direction[i];
		}
		double k = numerator / denominator;

		double d = 0;
		for (size_t i = 0; i<direction.size(); i += 1)		
			d += (k*direction[i] - point[i])*(k*direction[i] - point[i]);

		return sqrt(d);
	}
}