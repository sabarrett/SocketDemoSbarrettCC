#include "RayCaster.h"

std::vector<Vector2D> RayCaster::getPoints(const Vector2D& start, const Vector2D& end, float interval)
{
	std::vector<Vector2D> thePoints;

	Vector2D diff = end - start;
	Vector2D normalizedDiff = diff;
	normalizedDiff.normalize();

	int numIterations = (int)(diff.getLength() / interval)-1;
	for (int i = 1; i < numIterations; i++)
	{
		Vector2D point = (normalizedDiff * interval * (float)i) + start;
		thePoints.push_back(point);
	}
	return thePoints;
}
