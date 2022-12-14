#pragma once

#include <iostream>
#include <cmath>
#include <numbers>

#include "memory.h"
#include "Offsets.h"

#include <vector>
#include <Windows.h>

#include "variables.h"

struct Colour {
	constexpr Colour(float r, float g, float b, float a = 1.f) :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;


};



struct Vector3
{
	// constructor
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	// operator overloads
	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept
	{
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept
	{
		return Vector3{ x * factor, y * factor, z * factor };
	}

	// utils
	constexpr const Vector3& ToAngle() const noexcept
	{
		return Vector3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f };
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	// struct data
	float x, y, z;
};


constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(float _x, float _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	float x;
	float y;

	Vector2& operator+=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	bool equals(Vector2 other)
	{
		return (x == other.x && y == other.y);
	}

};

struct Vector4
{
	float x, y, z, w;
};





