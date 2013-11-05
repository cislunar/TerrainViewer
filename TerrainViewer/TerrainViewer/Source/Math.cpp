
#include <cmath>
#include <float.h>
#include <assert.h>
#include "Math.h"

// Vec2
Vec2 Vec2::Zero = Vec2(0,0);

float Vec2::operator[](int i) const {
	return (&x)[i];
}

float& Vec2::operator[](int i) {
	return (&x)[i];
}

Vec2 Vec2::operator+=(const Vec2 &v) {
	// *this = *this + v;
	x += v.x;
	y += v.y;
	return *this;
}

Vec2 Vec2::operator-=(const Vec2 &v) {
	// *this = *this + v;
	x -= v.x;
	y -= v.y;
	return *this;
}

Vec2 Vec2::operator*=(float s) {
	// *this = *this * s;
	x *= s;
	y *= s;
	return *this;
}

Vec2 Vec2::operator/=(float s) {
	// *this = *this * (1.0f / s);
	float r = 1.0f / s;
	x *= s;
	y *= s;
	return *this;
}

float Vec2::Length()
{
	return SQRT(x*x + y*y);
}

float Vec2::Length2()
{
	return x*x + y*y;
}

float Vec2::Normalize()
{
	float len = SQRT(x*x + y*y);
	x /= len;
	y /= len;
	return len;
}

float Vec2::SafeNormalize()
{
	float len = SQRT(x*x + y*y);
	const float EPSILON = 0.00001f;
	if( len > EPSILON )
	{
		x /= len;
		y /= len;
	}
	else
	{
		// Little or no length in line. Pretend there is a bit.
		y = 1;
		x = 0;
		const float fudge = 0.001f;
		len = fudge;
	}
	return len;
}

float Vec2::UnitFromDelta( const Vec2& posA, const Vec2& posB )
{
	x = posB.x - posA.x;
	y = posB.y - posA.y;

	float len = SQRT(x*x + y*y);
	const float EPSILON = 0.00001f;
	if( len > EPSILON )
	{
		x /= len;
		y /= len;
		return len;
	}
	else
	{
		// Little or no length in line. Pretend there is a bit.
		y = 1;
		x = 0;
		return 0.001f;	// Some small amount
	}
}

void Vec2::NanAssert()
{
	assert(!_isnan(x));
	assert(!_isnan(y));
}



Vec2 operator+(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x + v1.x, v0.y + v1.y);
}

Vec2 operator-(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x - v1.x, v0.y - v1.y);
}

Vec2 operator*(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x * v1.x, v0.y * v1.y);
}

Vec2 operator*(const Vec2 &v, float s)
{
	return Vec2(v.x * s, v.y * s);
}

Vec2 operator*(float s, const Vec2 &v)
{
	return v * s;
}

Vec2 operator/(const Vec2 &v, float s)
{
	return v * (1.0f / s);
}

float dot(const Vec2 &v0, const Vec2 &v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}


float Plane2D::distanceOfPoint(Vec2& point)
{
	float dist = dot( normal, point ) - distFromOrigin;
	return dist;
}
