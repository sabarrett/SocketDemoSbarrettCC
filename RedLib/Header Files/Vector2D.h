#pragma once
#include "Trackable.h"
#include <ostream>

class Vector2D : public Trackable
{

public:
	Vector2D();
	Vector2D(float x, float y);
	Vector2D(int x, int y);
	Vector2D(double x, double y);
	~Vector2D();

	Vector2D operator=(const Vector2D& other);

	Vector2D operator+(const Vector2D& other);
	Vector2D operator-(const Vector2D& other);
	Vector2D operator*(const float scalar) const;
	Vector2D operator/(const float scalar) const;
	Vector2D operator*(const int scalar) const;
	Vector2D operator/(const int scalar) const;
	Vector2D operator*(const double scalar) const;
	Vector2D operator/(const double scalar) const;

	Vector2D operator+=(const Vector2D& other);
	Vector2D operator-=(const Vector2D& other);
	Vector2D operator*=(const float scalar);
	Vector2D operator/=(const float scalar);

	bool operator==(const Vector2D& other) const;
	bool operator!=(const Vector2D& other) const;

	std::ostream& write(std::ostream& out) const;

	float length();
	void normalize();

	Vector2D normalized();

	float getX() const { return mX; }
	float getY() const { return mY; }

	static const Vector2D Zero() { return Vector2D(0, 0); }
	static const Vector2D Left() { return Vector2D(-1, 0); }
	static const Vector2D Right() { return Vector2D(1, 0); }
	static const Vector2D Up() { return Vector2D(0, -1); }
	static const Vector2D Down() { return Vector2D(0, 1); }

private:
	float mX, mY;
	
};

//global operator overloads (to get better implementation with ostream)
std::ostream& operator<<(std::ostream& out, Vector2D const &vec);
