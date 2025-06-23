#pragma once
#include <glm/vec3.hpp>

class MyVector
{
public:
	float x, y, z;

	MyVector();
	MyVector(float x, float y, float z);

	float Magnitude() const;

	MyVector Direction() const;

	MyVector operator+(const MyVector& other) const;

	MyVector operator-(const MyVector& other) const;

	MyVector operator*(float scalar) const;

	MyVector ComponentProduct(const MyVector& other) const;

	float ScalarProduct(const MyVector& other) const;

	MyVector VectorProduct(const MyVector& other) const;

	MyVector& operator*=(float scalar);

	friend MyVector operator*(float scalar, const MyVector& vector);

	operator glm::vec3() const
	{
		return glm::vec3(x, y, z);
	}

	MyVector& operator+=(const MyVector& force);
	float magnitude() const;
	MyVector normalize() const;

	MyVector& operator-=(const MyVector& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
};
