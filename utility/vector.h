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
* class vector defines basic operations for Euclidean vector operations
*
*********************************************************************************/
#ifndef OFEC_VECTOR_H
#define OFEC_MYVECTOR_H

#include <vector>
#include <fstream>
#include <random>

#include "../core/definition.h"
#include "random/newran.h"

namespace OFEC {

	class Vector final {
	
	private:
		std::vector<real> m_data;
		double m_length = 0;
		bool m_wrote = true;
	public:
		class vector_ {
		private:
			Vector & m_vec;
			size_t m_idx;
		public:
			vector_(Vector& v, size_t idx);
			vector_& operator=(const vector_& rhs);
			operator real() const;
			vector_&  operator=(real);
			vector_& operator+=(real);
			vector_& operator-=(real);
			vector_& operator/=(real);
			vector_& operator*=(real);
		};
		friend class vector_;
		Vector(size_t n = 0);
		Vector(size_t n, real val);
		Vector(size_t n, real *val);
		Vector(const std::vector<real> & v);
		Vector(const std::vector<real> &&v);
		Vector(const Vector&) = default;
		Vector(Vector&& rhs);
		Vector& operator=(Vector&& rhs);
		const real &operator [](size_t)const;

		std::vector<real> & data() { return m_data; }
		const std::vector<real> & data()const { return m_data; }

		Vector& operator =(const Vector & v);
		Vector& operator =(const std::vector<real> & v);
		Vector& operator +=(const Vector & v);
		Vector& operator -=(const Vector & v);
		Vector& operator *=(real val);
		Vector& operator /=(real val);
		Vector& operator -=(real val);
		Vector& operator +=(real val);
		Vector  operator *(real val) const;
		Vector  operator /(real val) const;
		Vector  operator -(real val)const;
		Vector  operator +(real val)const;
		real operator *(const Vector &v) const;


		Vector point_between(const Vector & v, double ratio)const;
		
		vector_ operator [](size_t);
		void push_back(real);
			
		Vector projection( const Vector &v) const;
		void normalize();

		size_t size()const;
		std::vector<real>::iterator  begin();
		std::vector<real>::iterator  end();

		void randomize_in_sphere(double radius, uniform * rand);
		void randomize_on_sphere(double radius, uniform * rand);
		void randomize_on_sphere(double radius, std::uniform_real_distribution<real> &unif, std::default_random_engine &gen);
		void randomize(uniform * rand,real min = 0, real max = 1);
		void randomize(std::uniform_real_distribution<real> &unif, std::default_random_engine &gen, real min, real max);	
		void randomize(normal *rand);
		void randomize_in_sphere(double radius, normal *nor, uniform * uni);

		double angle( Vector & v);
		double length();
		double distance(const Vector &point) const;
		double distance(const std::vector<real> &point)const;
		void zeroize();
		void resize(size_t n) noexcept;
		double perpendicular_distance(const Vector &point); 
	protected:
		void length_();
		friend std::ifstream &operator>>(std::ifstream &, Vector&);
	};

}

#endif
