#ifndef __POINT_H__
#define __POINT_H__

#include <cmath>
#include <compare>
#include <numbers>

template<class T>
class Point
{
public:

	T x = 0;
	T y = 0;

	Point& Create(const T& a, const T& b)
	{
		x = a;
		y = b;

		return *this;
	}

	// Math ------------------------------------------------
	Point operator -(const Point &v) const
	{
		Point r;

		r.x = x - v.x;
		r.y = y - v.y;

		return r;
	}

	Point operator +(const Point &v) const
	{
		Point r;

		r.x = x + v.x;
		r.y = y + v.y;

		return r;
	}

	Point operator +(T a) const
	{
		return {x + a, y + a};
	}

	Point operator -(T a) const
	{
		return {x - a, y - a};
	}

	Point operator *(T i)
	{
		Point r;

		r.x = x * i;
		r.y = y * i;

		return r;
	}

	Point operator /(const Point &v) const
	{
		return {x / v.x, y / v.y};
	}

	Point operator /(T i) const
	{
		return {x / i, y / i};
	}

	Point operator %(T i) const
	{
		return {x % i, y % i};
	}

	const Point& operator -=(const Point &v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	const Point& operator +=(const Point &v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	auto operator <=>(const Point &v) const = default;

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	Point Left()
	{
		return Point(x - 1, y);
	}

	Point Right()
	{
		return Point(x + 1, y);
	}

	Point Up()
	{
		return Point(x, y - 1);
	}

	Point Down()
	{
		return Point(x, y + 1);
	}

	// Distances ---------------------------------------------
	T DistanceTo(const Point& v) const
	{
		T fx = x - v.x;
		T fy = y - v.y;

		return sqrt((fx*fx) + (fy*fy));
	}

	T DistanceNoSqrt(const Point& v) const
	{
		T fx = x - v.x;
		T fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	T DistanceManhattan(const Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}

	float Degree(const Point &v) const
	{
		//Radiants
		return std::atan(static_cast<float>(v.y-y)/static_cast<float>(v.x-x)) * 360.0f/std::numbers::pi_v<float>;
	}
};

using iPoint = Point<int>;
using fPoint = Point<float>;
using uPoint = Point<unsigned int>;

#endif // __POINT_H__