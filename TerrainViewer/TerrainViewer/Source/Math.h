#pragma once

struct Vec2 {
	float x, y;

	Vec2()
	{}
	Vec2(float _x, float _y)
		: x(_x), y(_y)
	{}
	float operator[](int i) const;
	float& operator[](int i);
	Vec2 operator+=(const Vec2 &v);
	Vec2 operator-=(const Vec2 &v);
	Vec2 operator*=(float s);
	Vec2 operator/=(float s);

	float Length();
	float Length2();
	float Normalize();
	float SafeNormalize();
	void  NanAssert();
	float UnitFromDelta( const Vec2& posA, const Vec2& posB );

	static Vec2 Zero;
};

// vector ops
Vec2 operator-(const Vec2 &v);
Vec2 operator+(const Vec2 &v0, const Vec2 &v1);
Vec2 operator-(const Vec2 &v0, const Vec2 &v1);
Vec2 operator*(const Vec2 &v0, const Vec2 &v1);
Vec2 operator*(const Vec2 &v, float s);
Vec2 operator*(float s, const Vec2 &v);
Vec2 operator/(const Vec2 &v, float s);
float dot(const Vec2 &v0, const Vec2 &v1);


struct Plane2D
{
	Vec2 normal;
	float distFromOrigin;

	Plane2D( Vec2& norm, float dist )
		:
	normal(norm),
		distFromOrigin(dist)
	{}

	float distanceOfPoint(Vec2& point);
};


// FAST SQUARE ROOT
#if 1
// Normal crt version
#define SQRT sqrt
#else
// For Magic Derivation see: Chris Lomont http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
// Credited to Greg Walsh on Quake 3. Code downloaded from http://ilab.usc.edu/wiki/index.php/Fast_Square_Root
// 32  Bit float magic number 
#define SQRT_MAGIC_F 0x5f3759df
inline float invSqrt(const float x)
{
	// Quake 3 method
	const float xhalf = 0.5f*x;
	union // get bits for floating value
	{
		float x;
		int i;
	} u;
	u.x = x;
	u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
	return u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy
}
inline float fastSqrt(const float x)
{
  return x * invSqrt(x);
}
// On analysis, the Quake version actually seems slower in terms of resulting fps. Sticking with CRT version.
#define SQRT fastSqrt	// Quake method (supposed to be 2x faster, and still pretty accurate)
#endif