#pragma once

#include <stdlib.h>
#include <iostream>

class vector3
{
public: 
	vector3() = default;
	vector3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

	inline float x() const { return _x;  }
	inline float y() const { return _y; }
	inline float z() const { return _z; }

	inline float r() const { return _x; }
	inline float g() const { return _y; }
	inline float b() const { return _z; }

	inline const vector3& operator+() const { return *this; }
	inline vector3 operator-() const {
		return vector3(-_x, -_y, -_z);
	}

	inline vector3 operator+(const vector3& vec) const {
		return vector3(_x + vec._x, _y + vec._y, _z + vec._z);
	}

	inline vector3 operator-(const vector3& vec) const {
		return vector3(_x - vec._x, _y - vec._y, _z - vec._z);
	}

	inline vector3 operator*(const vector3& vec) const {
		return vector3(_x * vec._x, _y * vec._y, _z * vec._z);
	}

	inline vector3 operator/(const vector3& vec) const {
		return vector3(_x / vec._x, _y / vec._y, _z / vec._z);
	}

	inline const vector3& operator+=(const vector3& vec) { 
		_x += vec._x;
		_y += vec._y;
		_z += vec._z;
		return *this; 
	}
	
	inline const vector3& operator-=(const vector3& vec) {
		_x -= vec._x;
		_y -= vec._y;
		_z -= vec._z;
		return *this;
	}

	inline const vector3& operator*=(const vector3& vec) {
		_x *= vec._x;
		_y *= vec._y;
		_z *= vec._z;
		return *this;
	}

	inline const vector3& operator/=(const vector3& vec){
		_x /= vec._x;
		_y /= vec._y;
		_z /= vec._z;
		return *this;
	}

	inline const vector3& operator*=(float t) {
		_x *= t;
		_y *= t;
		_z *= t;
		return *this;
	}

	inline const vector3& operator/=(float t) {
		_x /= t;
		_y /= t;
		_z /= t;
		return *this;
	}

	inline float length() const {
		return sqrt((_x * _x) + (_y * _y) + (_z * _z));
	}

	inline float squared_length() const {
		return (_x * _x) + (_y * _y) + (_z * _z);
	}

	inline void make_unit_vector() {
		float k = 1.0f / length();
		_x *= k; 
		_y *= k;
		_z *= k;
	}

	inline std::istream& operator >>(std::istream& stream)
	{
		stream >> _x >> _y >> _z;
		return stream;
	}

	inline std::ostream& operator <<(std::ostream& stream)
	{
		stream << _x << " " << _y << " " << _z;
		return stream;
	}

	static vector3 ONE;
	static vector3 ZERO;
	static vector3 UP; 
private: 

	float _x = 0.0f, _y = 0.0f, _z = 0.0f;
};

inline float dot(const vector3& v1, const vector3& v2) {
	return (v1.x() * v2.x()) + (v1.y() * v2.y()) + (v1.z() * v2.z());
}

inline vector3 cross(const vector3& v1, const vector3& v2) {
	return vector3(
		(v1.y() * v2.z() - v1.z() * v2.y()),
		(-(v1.x() * v2.z() - v1.z() * v2.x())),
		(v1.x() * v2.y() - v1.y() * v2.x()));
}

inline vector3 operator*(const vector3& v, float t) {
	return vector3(v.x() * t, v.y() * t, v.z() * t);
}

inline vector3 operator*(float t, const vector3& v) {
	return vector3(t*v.x(), t * v.y(), t * v.z());
}


inline vector3 operator/(const vector3& v, float t) {
	return vector3(v.x() / t, v.y() / t, v.z() / t);
}

inline vector3 unit_vector(vector3 vec) {
	return vec / vec.length();
}
