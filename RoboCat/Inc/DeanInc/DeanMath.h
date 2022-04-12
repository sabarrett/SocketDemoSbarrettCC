#pragma once

class Vector2D;

const double PI = 3.14159265358979323846;
const double DOUBLE_PI = PI*2;
const double HALF_PI = PI / 2;
const double QUARTER_PI = PI / 2;

double mapToRangeMinusPiToPi(double val);

double calcDotProduct(const Vector2D& vector1, const Vector2D& vector2);

double lerp(double low, double high, double pct);
int lerp(int low, int high, double pct);
