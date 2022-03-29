#pragma once

/* Vector2D is a generally usable 2D vector.  Most of the operator overloading code is patterned after the notes from
	California Institue of Technology site: http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html .
	Exact author unknown.
	May be missing some important functions but this should be enough to do most things.

	by Dean Lawson
	Champlain College
	2011
*/

#include "Trackable.h"
#include "DeanMath.h"
#include <iostream>
#include <cfloat>


class Vector2D :public Trackable
{
	friend std::ostream& operator<< (std::ostream& out, const Vector2D& vector);

public:
	explicit Vector2D(float x = 0.0f, float y = 0.0f);//constructor
	explicit Vector2D(int x, int y);//constructor
	Vector2D(const Vector2D& rhs);//copy constructor
	~Vector2D();//destructor

	//math operators
	Vector2D& operator += (const Vector2D& rhs);
	Vector2D& operator -= (const Vector2D& rhs);
	Vector2D& operator *= (float mult);
	Vector2D& operator /= (float div);
	Vector2D& operator = (const Vector2D& rhs);

	bool operator == (const Vector2D& rhs)const;
	bool operator != (const Vector2D& rhs)const;

	Vector2D operator+(const Vector2D& other) const;
	Vector2D operator-(const Vector2D& other) const;
	Vector2D operator*(float mult) const;
	Vector2D operator/(float div) const;

	//getters and setters
	inline float getX() const { return mX; };
	inline float getY() const { return mY; };
	inline void setX(float x) { mX = x; };
	inline void setY(float y) { mY = y; };

	//length functions
	bool hasNonZeroLength() const;
	float getLength() const;
	float getLengthSquared() const;//more efficient than get length - use when comparing distances and actual distance is not needed

	void normalize();//makes vector a unit vector (length of 1)
	Vector2D getNormalizedVector() const;//returns a vector of length 1 but leaves the original vector unchanged
	float dotProduct(const Vector2D& other) const;
	Vector2D getRightVector() const;//right vector is a vector perpendicular and on the right side
	double calcFacing() const;
	static Vector2D getVectorInDirection(double direction);
	static Vector2D getVectorInOppositeDirection(const Vector2D& vec);
	static Vector2D getVectorInOppositeDirection(double direction);

	static float getDistanceBetween(const Vector2D& vec1, const Vector2D& vec2);
	static float getSquaredDistanceBetween(const Vector2D& vec1, const Vector2D& vec2);

private:
	float mX;
	float mY;
};

std::ostream& operator<< (std::ostream& out, const Vector2D& vector);

const Vector2D ZERO_VECTOR2D(0.0f, 0.0f);//useful when we need to return a pointer to a default vector from a function

