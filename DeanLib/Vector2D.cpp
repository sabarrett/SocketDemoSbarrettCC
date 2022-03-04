#include "Vector2D.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "include\DeanMath.h"

Vector2D::Vector2D(float x, float y)
	:mX(x)
	, mY(y)
{
}

Vector2D::Vector2D(const Vector2D& rhs)
	: mX(rhs.mX)
	, mY(rhs.mY)
{
}

Vector2D::Vector2D(int x, int y)
	: mX((float)x)
	, mY((float)y)
{
}

Vector2D::~Vector2D()
{
}

Vector2D& Vector2D::operator += (const Vector2D& rhs)
{
	mX += rhs.mX;
	mY += rhs.mY;
	return *this;
}

Vector2D& Vector2D::operator -= (const Vector2D& rhs)
{
	mX -= rhs.mX;
	mY -= rhs.mY;
	return *this;
}

Vector2D& Vector2D::operator = (const Vector2D& rhs)
{
	mX = rhs.mX;
	mY = rhs.mY;
	return *this;
}

Vector2D& Vector2D::operator *= (float mult)
{
	mX *= mult;
	mY *= mult;
	return *this;
}

Vector2D& Vector2D::operator /= (float div)
{
	mX /= div;
	mY /= div;
	return *this;
}

Vector2D Vector2D::operator+(const Vector2D& other) const
{
	Vector2D result = *this;
	result += other;
	return result;
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
	Vector2D result = *this;
	result -= other;
	return result;
}

Vector2D Vector2D::operator*(float mult) const
{
	Vector2D result = *this;
	result.mX *= mult;
	result.mY *= mult;

	return result;
}

Vector2D Vector2D::operator/(float div) const
{
	Vector2D result = *this;
	result.mX /= div;
	result.mY /= div;

	return result;
}

bool Vector2D::operator==(const Vector2D& rhs) const
{
	if ((getX() == rhs.getX()) && (getY() == rhs.getY()))
		return true;
	else return false;
}

bool Vector2D::operator!=(const Vector2D& rhs) const
{
	if ((getX() == rhs.getX()) && (getY() == rhs.getY()))
		return false;
	else return true;
}

bool Vector2D::hasNonZeroLength() const
{
	if (mX != 0.0f || mY != 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float Vector2D::getLength() const
{
	float lengthSquared = getLengthSquared();
	return sqrt(lengthSquared);
}

float Vector2D::getLengthSquared() const
{
	float lengthSquared = (mX * mX) + (mY * mY);
	return lengthSquared;
}

void Vector2D::normalize()
{
	float invLength = 1.0f / (getLength() + FLT_MIN);
	mX *= invLength;
	mY *= invLength;
}

Vector2D Vector2D::getNormalizedVector() const
{
	Vector2D newVector(*this);
	newVector.normalize();
	return newVector;
}

float Vector2D::dotProduct(const Vector2D& other) const
{
	return mX * other.mX + mY * other.mY;
}

Vector2D Vector2D::getRightVector() const
{
	return Vector2D(-mY, mX);
}

double Vector2D::calcFacing() const
{
	return atan2(mY, mX);
	//return atan2(mX, -mY);
}

Vector2D Vector2D::getVectorInDirection(double direction)
{
	Vector2D temp((float)cos(direction), (float)sin(direction));
	//std::cout << temp << " " << direction << std::endl;
	return temp;
	//return Vector2D((float)cos(direction), (float)sin(direction));
}

Vector2D Vector2D::getVectorInOppositeDirection(const Vector2D& vec)
{
	return Vector2D(-vec.getX(), -vec.getY());
}

Vector2D Vector2D::getVectorInOppositeDirection(double direction)
{
	return getVectorInOppositeDirection(getVectorInDirection(direction));
}

float Vector2D::getDistanceBetween(const Vector2D& vec1, const Vector2D& vec2)
{
	Vector2D temp(vec1 - vec2);
	return temp.getLength();
}

float Vector2D::getSquaredDistanceBetween(const Vector2D& vec1, const Vector2D& vec2)
{
	Vector2D temp(vec1 - vec2);
	return temp.getLengthSquared();
}

std::ostream& operator<<(std::ostream& out, const Vector2D& vector)
{
	out << "(" << vector.getX() << "," << vector.getY() << ")";
	return out;
}



