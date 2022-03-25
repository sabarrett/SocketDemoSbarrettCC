#include "DeanMath.h"
#include "Vector2D.h"

double calcDotProduct(const Vector2D& vector1, const Vector2D& vector2)
{
	return vector1.dotProduct(vector2);
}

double mapToRangeMinusPiToPi(double val)
{
	while (val < -PI)
	{
		val += DOUBLE_PI;
	}

	while (val > PI)
	{
		val -= DOUBLE_PI;
	}
	return val;
}

double lerp(double low, double high, double pct)
{
	if (pct > 1.0)
		pct = 1.0;
	else if (pct < 0.0)
		pct = 0.0;

	return low + pct * (high - low);
}

int lerp(int low, int high, double pct)
{
	if (pct > 1.0)
		pct = 1.0;
	else if (pct < 0.0)
		pct = 0.0;

	return low + (int)((float)pct * (high - low));
}

