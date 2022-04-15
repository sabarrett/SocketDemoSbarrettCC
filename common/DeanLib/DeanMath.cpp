#include "../../common/DeanLib/include/DeanMath.h"
#include "../../common/DeanLib/include/Vector2D.h"

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
