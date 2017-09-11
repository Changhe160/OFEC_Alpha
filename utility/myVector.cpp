#include "myVector.h"
#include "myexcept.h"
#include "../core/global.h"
  
namespace OFEC {

	myVector::myVectorProxy::myVectorProxy(myVector& v, int idx) :m_vec(v), m_idx(idx) {

	}
	myVector::myVectorProxy& myVector::myVectorProxy::operator=(const myVectorProxy& rhs) {
		m_vec.m_data[m_idx] = rhs.m_vec.m_data[rhs.m_idx];
		m_vec.m_write = true;
		return *this;
	}
	myVector::myVectorProxy::operator double()const {
		return m_vec.m_data[m_idx];
	}
	myVector::myVectorProxy&  myVector::myVectorProxy::operator=(double val) {
		m_vec.m_data[m_idx] = val;
		m_vec.m_write = true;
		return *this;
	}

	myVector::myVectorProxy & myVector::myVectorProxy::operator+=(double val) {
		m_vec.m_data[m_idx] += val;
		m_vec.m_write = true;
		return *this;
	}

	myVector::myVectorProxy & myVector::myVectorProxy::operator-=(double val) {
		m_vec.m_data[m_idx] -= val;
		m_vec.m_write = true;
		return *this;
	}

	myVector::myVectorProxy & myVector::myVectorProxy::operator*=(double val) {
		m_vec.m_data[m_idx] *= val;
		m_vec.m_write = true;
		return *this;
	}
	myVector::myVectorProxy & myVector::myVectorProxy::operator/=(double val) {
		m_vec.m_data[m_idx] /= val;
		m_vec.m_write = true;
		return *this;
	}
	myVector::myVector(const vector<typevar> &v) :m_length(0), m_write(true) {
		for (auto&i : v) {
			if (i.is_double()) {
				m_data.push_back(double(i));
			}
			else {
				throw myexcept("cannot assign non-double tpyeVar to double@myVector(const Solution &s)");
			}
		}
	}
	myVector::myVector(int size, double *val) :m_data(size), m_length(0), m_write(true) {
		copy(val, val + size, m_data.begin());
	}
	myVector::myVector(int size) : m_data(size), m_length(0), m_write(true) {

	}
	myVector::myVector(int size, double val) : m_data(size, val), m_length(0), m_write(true) {

	}
	myVector::myVector(const vector<double> & v) : m_data(v), m_length(0), m_write(true) {

	}
	myVector::~myVector() {
		m_data.clear();
	}
	myVector::myVectorProxy myVector::operator [](int idx) {
		return myVector::myVectorProxy(*this, idx);
	}
	const double & myVector::operator [](int idx)const {
		return m_data[idx];
	}
	myVector &myVector::operator =(const myVector & v) {
		if (this == &v) return *this;
		m_data = v.m_data;
		m_length = v.m_length;
		m_write = v.m_write;
		return *this;
	}
	myVector &  myVector::operator +=(const myVector & v) {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by + operation@myVector::operator +=");
		transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), plus<double>());
		m_write = true;
		return *this;
	}
	myVector &  myVector::operator -=(const myVector & v) {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by - operation@myVector::operator -=");
		transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), minus<double>());
		m_write = true;
		return *this;
	}
	double myVector::operator *(const myVector & v) {
		if (m_data.size() != v.m_data.size()) throw myexcept("the size of two vectors must be same by * operation@myVector::operator *");
		double sum = 0;
		for (unsigned i = 0; i < m_data.size(); i++) sum += m_data[i] * v.m_data[i];
		return sum;
	}
	void myVector::projectionToV(const myVector &v) {

		double sv = 0, vv = 0;
		for (unsigned i = 0; i < m_data.size(); i++) {
			sv += m_data[i] * v.m_data[i];
			vv += v.m_data[i] * v.m_data[i];
		}
		for (unsigned i = 0; i < m_data.size(); i++) {
			m_data[i] = sv*v.m_data[i] / vv;
		}
		m_write = true;
	}

	void myVector::normalize() {
		if (length() == 0) throw myexcept("vector length is zero @ myVector::normalize()");
		double sum = 0;
		for_each(m_data.begin(), m_data.end(), [&sum](double v) {sum += v*v; });
		sum = sqrt(sum);
		for_each(m_data.begin(), m_data.end(), [&sum](double &v) {v = v / sum; });
		m_length = 1;
		m_write = false;
	}

	void myVector::absoluteSum() {
		if (length() == 0) throw myexcept("vector length is zero @ myVector::normalize()");
		double sum = 0;
		for_each(m_data.begin(), m_data.end(), [&sum](double v) {sum += abs(v); });
		for_each(m_data.begin(), m_data.end(), [&sum](double &v) {v = v / sum; });
		m_length = 1;
		m_write = false;
	}

	int myVector::size()const {
		return m_data.size();
	}
	void myVector::randWithinRadi(double radius, caller mode) {
		if (global::ms_global.get() != nullptr) {
			double r = global::ms_global->getRandFloat(0, radius, mode);
			randomize(-1, 1, mode);
			normalize();
			for (auto&i : m_data) i *= r;
			m_write = true;
		}
		else {
			throw myexcept("global::msp_global not initialized@myVector::randWithinRadi()");
		}
	}
	void myVector::randOnRadi(double radius, caller mode) {
		if (global::ms_global.get() != nullptr) {
			randomize(-1, 1, mode);
			normalize();
			for (auto&i : m_data) i *= radius;
			m_length = radius;
			m_write = false;
		}
		else {
			throw myexcept("global::msp_global not initialized@myVector::randOnRadi()");
		}
	}

	void myVector::randOnRadi(double radius, uniform_real_distribution<double> &unif, default_random_engine &gen) {

		randomize(unif, gen, -1, 1);
		normalize();
		for (auto&i : m_data) i *= radius;
		m_length = radius;
		m_write = false;

	}
	void myVector::randomize(uniform_real_distribution<double> &unif, default_random_engine &gen, double min, double max) {
		for (auto&i : m_data) i = min + (unif(gen) - unif.min()) / (unif.max() - unif.min())*(max - min);
		m_write = true;
	}
	void myVector::randomize(double min, double max, caller mode) {
		if (global::ms_global.get() != nullptr) {
			for (auto&i : m_data) i = global::ms_global->getRandFloat(min, max, mode);
			m_write = true;
		}
		else {
			throw myexcept("global::msp_global not initialized@myVector::randomize()");
		}
	}
	void myVector::norRandomize(caller mode) {
		if (global::ms_global.get() != nullptr) {
			if (mode == caller::Algorithm)		for (auto&i : m_data) i = global::ms_global->m_normalAlg->next();
			else for (auto&i : m_data) i = global::ms_global->m_normalPro->next();
			m_write = true;
		}
		else {
			throw myexcept("global::msp_global not initialized@myVector::norRandomize()");
		}
	}
	void myVector::norRandWithinRadi(double radius, caller mode) {
		if (global::ms_global.get() != nullptr) {
			double r = 0;
			if (mode == caller::Algorithm)		r = fabs(global::ms_global->m_normalAlg->next_non_standard(0, radius));
			else r = fabs(global::ms_global->m_normalPro->next_non_standard(0, radius));
			randomize(-1, 1, mode);
			normalize();
			for (auto&i : m_data) i *= r;
			m_write = true;
		}
		else {
			throw myexcept("global::msp_global not initialized@myVector::randWithinRadi()");
		}
	}
	myVector &myVector::operator =(const vector<double> & v) {
		if (m_data.size() == v.size()) {
			m_data = v;
			m_write = true;
		}
		else {
			throw myexcept("size not the same @ myVector::operator =(vector<double>&)");
		}
		return *this;
	}

	myVector & myVector::operator *=(double val) {
		for (auto &i : m_data) i *= val;
		m_write = true;
		return *this;
	}
	myVector & myVector::operator /=(double val) {
		for (auto &i : m_data) i /= val;
		m_write = true;
		return *this;
	}
	myVector & myVector::operator -=(double val) {
		for (auto &i : m_data) i -= val;
		m_write = true;
		return *this;
	}
	myVector & myVector::operator +=(double val) {
		for (auto &i : m_data) i += val;
		m_write = true;
		return *this;
	}
	vector<double>::iterator  myVector::begin() {
		return m_data.begin();
	}
	vector<double>::iterator  myVector::end() {
		return m_data.end();
	}
	myVector  myVector::operator *(double val)const {
		vector<double> v(m_data);
		for (auto&i : v) i *= val;
		return myVector(v);
	}
	myVector  myVector::operator *(double val) {
		vector<double> v(m_data);
		for (auto&i : v) i *= val;
		return myVector(v);
	}
	myVector  myVector::operator /(double val) {
		vector<double> v(m_data);
		for (auto&i : v) i /= val;
		return myVector(v);
	}
	myVector  myVector::operator -(double val) {
		vector<double> v(m_data);
		for (auto&i : v) i -= val;
		return myVector(v);
	}
	myVector  myVector::operator +(double val) {
		vector<double> v(m_data);
		for (auto&i : v) i += val;
		return myVector(v);
	}
	double myVector::getAngle(myVector & v) {
		double sum = this->operator*(v);
		return acos(sum / (length()*v.length()));
	}

	void myVector::push_back(double n) {
		m_data.push_back(n);
		m_write = true;
	}

	double myVector::getDis(const myVector&v) {
		if (v.size() != size()) throw myexcept("size not the same@  myVector::getDis(const myVector&v)");
		myVector vec(m_data);
		vec -= v;
		return vec.length();
	}
	double myVector::getDis(const vector<double>&v) {
		if (v.size() != size()) throw myexcept("size not the same@  myVector::getDis(const myVector&v)");
		myVector vec(m_data);
		vec -= v;
		return vec.length();
	}

	ifstream &operator>>(ifstream & in, myVector&v) {
		for (auto &i : v.m_data) {
			in>> i;
		}
		v.m_write = true;
		return in;
	}
	double myVector::length() {
		if (m_write) {
			length_();
			m_write = false;
		}
		return m_length;
	}
	myVector::myVector(myVector&& rhs) :m_data(move(rhs.m_data)), m_length(rhs.m_length), m_write(rhs.m_write) {
		rhs.m_length = 0;
		rhs.m_write = false;
	}

	myVector& myVector::operator=(myVector&& rhs) {
		m_data = move(rhs.m_data);
		m_length = rhs.m_length;
		m_write = rhs.m_write;
		rhs.m_length = 0;
		rhs.m_write = false;
		return *this;
	}

	void myVector::zero() {
		for (auto &i : m_data) {
			i = 0;
		}
		m_length = 0;
		m_write = false;
	}

	myVector myVector::getPointBetween(const myVector & v1, double r) {
		vector<double> v(m_data);
		for (auto i = 0; i < m_data.size(); ++i) {
			v[i] = m_data[i] * r + v1[i] * (1 - r);
		}
		return myVector(v);
	}
} 