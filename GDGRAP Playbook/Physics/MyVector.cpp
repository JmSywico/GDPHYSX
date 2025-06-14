#include "MyVector.h"
#include <cmath>

MyVector::MyVector() : x(0.0f), y(0.0f), z(0.0f)
{
}

MyVector::MyVector(float x, float y, float z) : x(x), y(y), z(z)
{
}

float MyVector::Magnitude() const
{
	return std::sqrt(x * x + y * y + z * z);
}

MyVector MyVector::Direction() const
{
	float mag = Magnitude();
	if (mag == 0.0f) return MyVector(0.0f, 0.0f, 0.0f);
	return MyVector(x / mag, y / mag, z / mag);
}

MyVector MyVector::operator+(const MyVector& other) const
{
	return MyVector(x + other.x, y + other.y, z + other.z);
}

MyVector MyVector::operator-(const MyVector& other) const
{
	return MyVector(x - other.x, y - other.y, z - other.z);
}

MyVector MyVector::operator*(float scalar) const
{
	return MyVector(x * scalar, y * scalar, z * scalar);
}

MyVector MyVector::ComponentProduct(const MyVector& other) const
{
	return MyVector(x * other.x, y * other.y, z * other.z);
}

float MyVector::ScalarProduct(const MyVector& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

MyVector MyVector::VectorProduct(const MyVector& other) const
{
	return MyVector(
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x
	);
}

MyVector operator*(float scalar, const MyVector& vector)
{
	return MyVector(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

MyVector& MyVector::operator+=(const MyVector& force)
{
	this->x += force.x;
	this->y += force.y;
	this->z += force.z;
	return *this;
}

float MyVector::magnitude() const
{
	return std::sqrt(x * x + y * y + z * z);
}

MyVector MyVector::normalize() const
{
	float mag = magnitude();
	if (mag == 0.0f)
	{
		return MyVector(0, 0, 0); // Return a zero vector if magnitude is zero
	}
	return MyVector(x / mag, y / mag, z / mag); // Normalize the vector
}

MyVector& MyVector::operator*=(float scalar)
{
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;
	return *this;
}
