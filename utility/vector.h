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
#define OFEC_VECTOR_H

#include <vector>
#include <fstream>
#include <random>

#include "../core/definition.h"
#include "random/newran.h"

namespace OFEC {

	class Vector final {
		friend Vector operator*(const real val, const Vector & vec);
		friend Vector operator*(const Vector & vec, const real val);
		friend Vector operator/(const real val, const Vector & vec);
		friend Vector operator+(const real val, const Vector & vec);
		friend Vector operator+(const Vector & vec, const real val);
		friend Vector operator-(const real val, const Vector & vec);
		friend Vector operator-(const Vector & vec, const real val);
		friend Vector operator+(const Vector &left_vec, const Vector &righr_vec);
		friend Vector operator-(const Vector &left_vec, const Vector &righr_vec);
		friend real operator*(const Vector &left_vec, const Vector &righr_vec);
	private:
		std::vector<real> m_data;
		real m_length = 0;
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
		Vector(const std::vector<real> & v);
		Vector(const std::vector<real> &&v);
		Vector(const Vector&) = default;
		Vector(Vector&& rhs);
		Vector& operator=(Vector&& rhs);
		const real &operator [](size_t)const;

		std::vector<real> & vect() { return m_data; }
		const std::vector<real> & vect()const { return m_data; }

		Vector& operator =(const Vector & v);
		Vector& operator =(const std::vector<real> & v);
		Vector& operator +=(const Vector & v);
		Vector& operator -=(const Vector & v);
		Vector& operator *=(real val);
		Vector& operator /=(real val);
		Vector& operator -=(real val);
		Vector& operator +=(real val);

		Vector point_between(const Vector & v, real ratio)const;
		
		vector_ operator [](size_t);
		void push_back(real);
			
		Vector projection( const Vector &v) const;
		void normalize();

		size_t size()const;
		std::vector<real>::iterator  begin();
		std::vector<real>::iterator  end();

		void randomize_in_sphere(real radius, uniform * rand);
		void randomize_on_sphere(real radius, uniform * rand);
		void randomize_on_sphere(real radius, std::uniform_real_distribution<real> &unif, std::default_random_engine &gen);
		void randomize(uniform * rand,real min = 0, real max = 1);
		void randomize(std::uniform_real_distribution<real> &unif, std::default_random_engine &gen, real min, real max);	
		void randomize(normal *rand);
		void randomize_in_sphere(real radius, normal *nor, uniform * uni);

		real angle( Vector & v);
		real length();
		real distance(const Vector &point) const;
		real distance(const std::vector<real> &point)const;
		void zeroize();
		void resize(size_t n) noexcept;
		real perpendicular_distance(const Vector &point); 
	protected:
		void length_();
		friend std::ifstream &operator>>(std::ifstream &, Vector&);
	};

}

#endif
